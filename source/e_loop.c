
#include "event/e_event.h"
#include "event/e_loop.h"
#include "event/e_alloc.h"
#include "event/e_nio.h"
#include "util/e_time.h"
#include "e_watcher.h"

#define EVENT_LOOP_PAUSE_TIME        10      // ms
#define EVENT_LOOP_MAX_BLOCK_TIME    100     // ms
#define EVENT_LOOP_STAT_TIMEOUT      60000   // ms

#define IO_ARRAY_INIT_SIZE              1024
#define CUSTOM_EVENT_QUEUE_INIT_SIZE    16

static void e_loop_destroy_sockpair(e_loop_t *loop) {
  SAFE_CLOSESOCKET(loop->sockpair[SOCKPAIR_READ_INDEX]);
  SAFE_CLOSESOCKET(loop->sockpair[SOCKPAIR_WRITE_INDEX]);
}

static void e_loop_cleanup(e_loop_t *loop) {
  // pendings
  printd("cleanup pendings...\n");
  for (int i = 0; i < EVENT_PRIORITY_SIZE; ++i) {
    loop->pendings[i] = NULL;
  }

  // ios
  printd("cleanup ios...\n");
  for (int i = 0; i < loop->ios.maxsize; ++i) {
    e_io_t *io = loop->ios.ptr[i];
    if (io) {
      e_io_free(io);
    }
  }
  io_array_cleanup(&loop->ios);

  // idles
  printd("cleanup idles...\n");
  struct list_node *node = loop->idles.next;
  e_idle_t *idle;
  while (node != &loop->idles) {
    idle = IDLE_ENTRY(node);
    node = node->next;
    EVENT_FREE(idle);
  }
  list_init(&loop->idles);

  // timers
  printd("cleanup timers...\n");
  e_timer_t *timer;
  while (loop->timers.root) {
    timer = TIMER_ENTRY(loop->timers.root);
    heap_dequeue(&loop->timers);
    EVENT_FREE(timer);
  }
  heap_init(&loop->timers, NULL);

  // readbuf
  if (loop->readbuf.base && loop->readbuf.len) {
    EVENT_FREE(loop->readbuf.base);
    loop->readbuf.base = NULL;
    loop->readbuf.len = 0;
  }

  // iowatcher
  iowatcher_cleanup(loop);

  // custom_events
  e_mutex_lock(&loop->custom_events_mutex);
  e_loop_destroy_sockpair(loop);
  e_queue_cleanup(&loop->custom_events);
  e_mutex_unlock(&loop->custom_events_mutex);
  e_mutex_destroy(&loop->custom_events_mutex);
}

static int timers_compare(const struct heap_node *lhs, const struct heap_node *rhs) {
  return TIMER_ENTRY(lhs)->next_timeout < TIMER_ENTRY(rhs)->next_timeout;
}

static void eloop_init(e_loop_t *loop) {
#ifdef EVENT_OS_WIN
  WSAInit();
#endif
#ifdef SIGPIPE
  // NOTE: if not ignore SIGPIPE, write twice when peer close will lead to exit process by SIGPIPE.
    signal(SIGPIPE, SIG_IGN);
#endif
  loop->status = EVENT_LOOP_STATUS_STOP;
  loop->pid = e_getpid();
  loop->tid = e_gettid();

  // idles
  list_init(&loop->idles);

  // timers
  heap_init(&loop->timers, timers_compare);

  // ios
  io_array_init(&loop->ios, IO_ARRAY_INIT_SIZE);

  // readbuf
  loop->readbuf.len = EVENT_LOOP_READ_BUFSIZE;
  EVENT_ALLOC(loop->readbuf.base, loop->readbuf.len);

  // iowatcher
  iowatcher_init(loop);

  // custom_events
  e_mutex_init(&loop->custom_events_mutex);
  e_queue_init(&loop->custom_events, CUSTOM_EVENT_QUEUE_INIT_SIZE);


  // NOTE: hloop_create_sockpair when hloop_post_event or hloop_run
  loop->sockpair[0] = loop->sockpair[1] = -1;

  // NOTE: init start_time here, because htimer_add use it.
  loop->start_ms = gettimeofday_ms();
  loop->start_hrtime = loop->cur_hrtime = gethrtime_us();
}

static void sockpair_read_cb(e_io_t *io, void *buf, int readbytes) {
  e_loop_t *loop = io->loop;
  e_event_t *pev = NULL;
  e_event_t ev;
  for (int i = 0; i < readbytes; ++i) {
    e_mutex_lock(&loop->custom_events_mutex);
    if (e_queue_empty(&loop->custom_events)) {
      goto unlock;
    }
    pev = e_queue_front(&loop->custom_events);
    if (pev == NULL) {
      goto unlock;
    }
    ev = *pev;
    e_queue_pop_front(&loop->custom_events);
    // NOTE: unlock before cb, avoid deadlock if hloop_post_event called in cb.
    e_mutex_unlock(&loop->custom_events_mutex);
    if (ev.cb) {
      ev.cb(&ev);
    }
  }
  return;
  unlock:
  e_mutex_unlock(&loop->custom_events_mutex);
}

int e_loop_create_sockpair(e_loop_t *loop) {
#if defined(EVENT_OS_LINUX)
  if (pipe(loop->sockpair) != 0) {
//        hloge("pipe create failed!");
    return -1;
  }
#else
  if (Socketpair(AF_INET, SOCK_STREAM, 0, loop->sockpair) != 0) {
    hloge("socketpair create failed!");
    return -1;
  }
#endif
  e_io_t
      *io = e_read(loop, loop->sockpair[SOCKPAIR_READ_INDEX], loop->readbuf.base, loop->readbuf.len, sockpair_read_cb);
  io->priority = EVENT_HIGH_PRIORITY;
  // NOTE: Avoid duplication closesocket in hio_cleanup
  loop->sockpair[SOCKPAIR_READ_INDEX] = -1;
  ++loop->intern_nevents;
  return 0;
}

e_loop_t *e_loop_new(int flags DEFAULT(ELOOP_FLAG_AUTO_FREE)) {
  e_loop_t *loop;
  EVENT_ALLOC_SIZEOF(loop);
  eloop_init(loop);
  loop->flags |= flags;
  return loop;
}

e_io_t *e_loop_create_tcp_server(e_loop_t *loop, const char *host, int port, e_accept_cb accept_cb) {
  e_io_t *io = e_io_create_socket(loop, host, port, EVENT_IO_TYPE_TCP, EVENT_IO_SERVER_SIDE);
  if (io == NULL) return NULL;
  e_io_setcb_accept(io, accept_cb);
  e_io_accept(io);
  return io;
}

e_io_t *e_read(e_loop_t *loop, int fd, void *buf, size_t len, e_read_cb read_cb) {
  e_io_t *io = e_io_get(loop, fd);
  assert(io != NULL);
  if (buf && len) {
    io->readbuf.base = (char *) buf;
    io->readbuf.len = len;
  }
  if (read_cb) {
    io->read_cb = read_cb;
  }
  e_io_read(io);
  return io;
}
e_io_t *e_write(e_loop_t *loop, int fd, const void *buf, size_t len, e_write_cb write_cb DEFAULT(NULL)) {
  e_io_t *io = e_io_get(loop, fd);
  assert(io != NULL);
  if (write_cb) {
    io->write_cb = write_cb;
  }
  e_io_write(io, buf, len);
  return io;
}
void e_close(e_loop_t *loop, int fd) {
  e_io_t *io = e_io_get(loop, fd);
  assert(io != NULL);
  e_io_close(io);
}
static int e_loop_process_ios(e_loop_t *loop, int timeout) {
  // That is to call IO multiplexing function such as select, poll, epoll, etc.
  int nevents = iowatcher_poll_events(loop, timeout);
  if (nevents < 0) {
//    hlogd("poll_events error=%d", -nevents);
  }
  return nevents < 0 ? 0 : nevents;
}
static void __e_timer_del(e_timer_t *timer) {
  if (timer->destroy) return;
  heap_remove(&timer->loop->timers, &timer->node);
  timer->loop->ntimers--;
  timer->destroy = 1;
}

static int e_loop_process_timers(e_loop_t *loop) {
  int ntimers = 0;
  e_timer_t *timer = NULL;
  uint64_t now_hrtime = e_loop_now_hrtime(loop);
  while (loop->timers.root) {
    // NOTE: root of minheap has min timeout.
    timer = TIMER_ENTRY(loop->timers.root);
    if (timer->next_timeout > now_hrtime) {
      break;
    }
    if (timer->repeat != INFINITE) {
      --timer->repeat;
    }
    if (timer->repeat == 0) {
      // NOTE: Just mark it as destroy and remove from heap.
      // Real deletion occurs after hloop_process_pendings.
      __e_timer_del(timer);
    } else {
      // NOTE: calc next timeout, then re-insert heap.
      heap_dequeue(&loop->timers);
      if (timer->event_type == EVENT_TYPE_TIMEOUT) {
        while (timer->next_timeout <= now_hrtime) {
          timer->next_timeout += (uint64_t) ((e_timeout_t *) timer)->timeout * 1000;
        }
      } else if (timer->event_type == EVENT_TYPE_PERIOD) {
        e_period_t *period = (e_period_t *) timer;
        timer->next_timeout = (uint64_t) cron_next_timeout(period->minute, period->hour, period->day,
                                                           period->week, period->month) * 1000000;
      }
      heap_insert(&loop->timers, &timer->node);
    }
    EVENT_PENDING(timer);
    ++ntimers;
  }
  return ntimers;
}
static void __e_idle_del(e_idle_t *idle) {
  if (idle->destroy) return;
  idle->destroy = 1;
  list_del(&idle->node);
  idle->loop->nidles--;
}
static int e_loop_process_idles(e_loop_t *loop) {
  int nidles = 0;
  struct list_node *node = loop->idles.next;
  e_idle_t *idle = NULL;
  while (node != &loop->idles) {
    idle = IDLE_ENTRY(node);
    node = node->next;
    if (idle->repeat != INFINITE) {
      --idle->repeat;
    }
    if (idle->repeat == 0) {
      // NOTE: Just mark it as destroy and remove from list.
      // Real deletion occurs after hloop_process_pendings.
      __e_idle_del(idle);
    }
    EVENT_PENDING(idle);
    ++nidles;
  }
  return nidles;
}

static int e_loop_process_pendings(e_loop_t *loop) {
  if (loop->npendings == 0) return 0;

  e_event_t *cur = NULL;
  e_event_t *next = NULL;
  int ncbs = 0;
  // NOTE: invoke event callback from high to low sorted by priority.
  for (int i = EVENT_PRIORITY_SIZE - 1; i >= 0; --i) {
    cur = loop->pendings[i];
    while (cur) {
      next = cur->pending_next;
      if (cur->pending) {
        if (cur->active && cur->cb) {
          cur->cb(cur);
          ++ncbs;
        }
        cur->pending = 0;
        // NOTE: Now we can safely delete event marked as destroy.
        if (cur->destroy) {
          EVENT_DEL(cur);
        }
      }
      cur = next;
    }
    loop->pendings[i] = NULL;
  }
  loop->npendings = 0;
  return ncbs;
}
static int e_loop_process_events(e_loop_t *loop) {
  // ios -> timers -> idles
  int nios, ntimers, nidles;
  nios = ntimers = nidles = 0;

  // calc blocktime
  int32_t blocktime = EVENT_LOOP_MAX_BLOCK_TIME;
  if (loop->timers.root) {
    e_loop_update_time(loop);
    uint64_t next_min_timeout = TIMER_ENTRY(loop->timers.root)->next_timeout;
    int64_t blocktime_us = next_min_timeout - e_loop_now_hrtime(loop);
    if (blocktime_us <= 0)
      goto process_timers;
    blocktime = blocktime_us / 1000;
    ++blocktime;
    blocktime = MIN(blocktime, EVENT_LOOP_MAX_BLOCK_TIME);
  }

  if (loop->nios) {
    nios = e_loop_process_ios(loop, blocktime);
  } else {
    e_msleep(blocktime);
  }
  e_loop_update_time(loop);
  // wakeup by hloop_stop
  if (loop->status == EVENT_LOOP_STATUS_STOP) {
    return 0;
  }

process_timers:
  if (loop->ntimers) {
    ntimers = e_loop_process_timers(loop);
  }

  int npendings = loop->npendings;
  if (npendings == 0) {
    if (loop->nidles) {
      nidles = e_loop_process_idles(loop);
    }
  }
  int ncbs = e_loop_process_pendings(loop);
  // printd("blocktime=%d nios=%d/%u ntimers=%d/%u nidles=%d/%u nactives=%d npendings=%d ncbs=%d\n",
  //         blocktime, nios, loop->nios, ntimers, loop->ntimers, nidles, loop->nidles,
  //         loop->nactives, npendings, ncbs);
  return ncbs;
}

int e_loop_run(e_loop_t *loop) {
  if (loop == NULL) return -1;
  if (loop->status == EVENT_LOOP_STATUS_RUNNING) return -2;

  loop->status = EVENT_LOOP_STATUS_RUNNING;
  loop->pid = e_getpid();
  loop->tid = e_gettid();

  if (loop->intern_nevents == 0) {
    e_mutex_lock(&loop->custom_events_mutex);
    if (loop->sockpair[SOCKPAIR_WRITE_INDEX] == -1) {
      e_loop_create_sockpair(loop);
    }
    e_mutex_unlock(&loop->custom_events_mutex);

#ifdef DEBUG
    htimer_add(loop, hloop_stat_timer_cb, HLOOP_STAT_TIMEOUT, INFINITE);
        ++loop->intern_nevents;
#endif
  }

  while (loop->status != EVENT_LOOP_STATUS_STOP) {
    if (loop->status == EVENT_LOOP_STATUS_PAUSE) {
      e_msleep(EVENT_LOOP_PAUSE_TIME);
      e_loop_update_time(loop);
      continue;
    }
    ++loop->loop_cnt;
    if ((loop->flags & EVENT_LOOP_FLAG_QUIT_WHEN_NO_ACTIVE_EVENTS) &&
        loop->nactives <= loop->intern_nevents) {
      break;
    }
    e_loop_process_events(loop);
    if (loop->flags & EVENT_LOOP_FLAG_RUN_ONCE) {
      break;
    }
  }

  loop->status = EVENT_LOOP_STATUS_STOP;
  loop->end_hrtime = gethrtime_us();

  if (loop->flags & EVENT_LOOP_FLAG_AUTO_FREE) {
    e_loop_cleanup(loop);
    EVENT_FREE(loop);
  }
  return 0;
}

void e_loop_update_time(e_loop_t *loop) {
  loop->cur_hrtime = gethrtime_us();
  if (ABS((int64_t) e_loop_now(loop) - (int64_t) time(NULL)) > 1) {
    // systemtime changed, we adjust start_ms
    loop->start_ms = gettimeofday_ms() - (loop->cur_hrtime - loop->start_hrtime) / 1000;
  }
}
uint64_t e_loop_now(e_loop_t *loop) {
  return loop->start_ms / 1000 + (loop->cur_hrtime - loop->start_hrtime) / 1000000;
}
uint64_t e_loop_now_ms(e_loop_t *loop) {
  return loop->start_ms + (loop->cur_hrtime - loop->start_hrtime) / 1000;
}
uint64_t e_loop_now_hrtime(e_loop_t *loop) {
  return loop->start_ms * 1000 + (loop->cur_hrtime - loop->start_hrtime);
}

void e_loop_free(e_loop_t **pp) {
  if (pp && *pp) {
    e_loop_cleanup(*pp);
    EVENT_FREE(*pp);
    *pp = NULL;
  }
}