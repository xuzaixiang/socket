//
// Created by 20123460 on 2022/1/13.
//

#include "event/e_loop.h"


static void e_loop_cleanup(e_loop_t* loop) {
  // pendings
//  printd("cleanup pendings...\n");
//  for (int i = 0; i < HEVENT_PRIORITY_SIZE; ++i) {
//    loop->pendings[i] = NULL;
//  }
//
//  // ios
//  printd("cleanup ios...\n");
//  for (int i = 0; i < loop->ios.maxsize; ++i) {
//    hio_t* io = loop->ios.ptr[i];
//    if (io) {
//      hio_free(io);
//    }
//  }
//  io_array_cleanup(&loop->ios);
//
//  // idles
//  printd("cleanup idles...\n");
//  struct list_node* node = loop->idles.next;
//  hidle_t* idle;
//  while (node != &loop->idles) {
//    idle = IDLE_ENTRY(node);
//    node = node->next;
//    HV_FREE(idle);
//  }
//  list_init(&loop->idles);
//
//  // timers
//  printd("cleanup timers...\n");
//  htimer_t* timer;
//  while (loop->timers.root) {
//    timer = TIMER_ENTRY(loop->timers.root);
//    heap_dequeue(&loop->timers);
//    HV_FREE(timer);
//  }
//  heap_init(&loop->timers, NULL);
//
//  // readbuf
//  if (loop->readbuf.base && loop->readbuf.len) {
//    HV_FREE(loop->readbuf.base);
//    loop->readbuf.base = NULL;
//    loop->readbuf.len = 0;
//  }
//
//  // iowatcher
//  iowatcher_cleanup(loop);
//
//  // custom_events
//  hmutex_lock(&loop->custom_events_mutex);
//  hloop_destroy_eventfds(loop);
//  event_queue_cleanup(&loop->custom_events);
//  hmutex_unlock(&loop->custom_events_mutex);
//  hmutex_destroy(&loop->custom_events_mutex);
}

static int e_loop_process_ios(e_loop_t* loop, int timeout) {
  // That is to call IO multiplexing function such as select, poll, epoll, etc.
  int nevents = iowatcher_poll_events(loop, timeout);
  if (nevents < 0) {
//    hlogd("poll_events error=%d", -nevents);
  }
  return nevents < 0 ? 0 : nevents;
}

// hloop_process_ios -> hloop_process_timers -> hloop_process_idles -> hloop_process_pendings
static int e_loop_process_events(e_loop_t* loop) {
  // ios -> timers -> idles
  int nios, ntimers, nidles;
  nios = ntimers = nidles = 0;

  // calc blocktime
//  int32_t blocktime = EVENT_LOOP_MAX_BLOCK_TIME;
//  if (loop->timers.root) {
//    hloop_update_time(loop);
//    uint64_t next_min_timeout = TIMER_ENTRY(loop->timers.root)->next_timeout;
//    int64_t blocktime_us = next_min_timeout - hloop_now_hrtime(loop);
//    if (blocktime_us <= 0) goto process_timers;
//    blocktime = blocktime_us / 1000;
//    ++blocktime;
//    blocktime = MIN(blocktime, HLOOP_MAX_BLOCK_TIME);
//  }
//
  if (loop->nios) {
    nios = e_loop_process_ios(loop, -1);
  } else {
//    hv_msleep(blocktime);
  }
//  hloop_update_time(loop);
//  // wakeup by hloop_stop
//  if (loop->status == HLOOP_STATUS_STOP) {
//    return 0;
//  }
//
//  process_timers:
//  if (loop->ntimers) {
//    ntimers = hloop_process_timers(loop);
//  }
//
//  int npendings = loop->npendings;
//  if (npendings == 0) {
//    if (loop->nidles) {
//      nidles= hloop_process_idles(loop);
//    }
//  }
//  int ncbs = hloop_process_pendings(loop);
//  // printd("blocktime=%d nios=%d/%u ntimers=%d/%u nidles=%d/%u nactives=%d npendings=%d ncbs=%d\n",
//  //         blocktime, nios, loop->nios, ntimers, loop->ntimers, nidles, loop->nidles,
//  //         loop->nactives, npendings, ncbs);
//  return ncbs;
}

static void e_loop_init(e_loop_t *loop) {
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
  io_array_init(&loop->ios, EVENT_IO_ARRAY_INIT_SIZE);

  iowatcher_init(loop);
}

e_loop_t *e_loop_new(uint32_t flags) {
  e_loop_t *loop;
  EVENT_ALLOC_SIZEOF(loop);
  e_loop_init(loop);
  loop->flags |= flags;
  return loop;
}
int e_loop_run(e_loop_t *loop) {
  if (loop == NULL) return -1;
  if (loop->status == EVENT_LOOP_STATUS_RUNNING) return -2;
  loop->status = EVENT_LOOP_STATUS_RUNNING;
  loop->pid = e_getpid();
  loop->tid = e_gettid();

  while (loop->status != EVENT_LOOP_STATUS_STOP) {
//    if (loop->status == EVENT_LOOP_STATUS_PAUSE) {
//      e_msleep(EVENT_LOOP_PAUSE_TIME);
//      hloop_update_time(loop);
//      continue;
//    }
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
  if (loop->flags & EVENT_LOOP_FLAG_AUTO_FREE) {
    e_loop_cleanup(loop);
    EVENT_FREE(loop);
  }
  return 0;
}