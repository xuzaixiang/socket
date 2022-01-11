
#include "event/e_loop.h"
#include "event/e_alloc.h"
#include "event/e_event.h"
#include "os.h"

static void eloop_init(e_loop_t* loop) {
#ifdef EVENT_OS_WIN
  WSAInit();
#endif
#ifdef SIGPIPE
  // NOTE: if not ignore SIGPIPE, write twice when peer close will lead to exit process by SIGPIPE.
    signal(SIGPIPE, SIG_IGN);
#endif
  loop->status = ELOOP_STATUS_STOP;
  loop->pid = e_getpid();
  loop->tid = e_gettid();

  // idles
  list_init(&loop->idles);
//
//  // timers
//  heap_init(&loop->timers, timers_compare);
//
//  // ios
//  io_array_init(&loop->ios, IO_ARRAY_INIT_SIZE);
//
//  // readbuf
//  loop->readbuf.len = HLOOP_READ_BUFSIZE;
//  HV_ALLOC(loop->readbuf.base, loop->readbuf.len);
//
//  // iowatcher
//  iowatcher_init(loop);
//
//  // custom_events
//  hmutex_init(&loop->custom_events_mutex);
//  event_queue_init(&loop->custom_events, CUSTOM_EVENT_QUEUE_INIT_SIZE);


  // NOTE: hloop_create_sockpair when hloop_post_event or hloop_run
//  loop->sockpair[0] = loop->sockpair[1] = -1;

  // NOTE: init start_time here, because htimer_add use it.
//  loop->start_ms = gettimeofday_ms();
//  loop->start_hrtime = loop->cur_hrtime = gethrtime_us();
}

e_loop_t *e_loop_new(int flags DEFAULT(ELOOP_FLAG_AUTO_FREE)) {
  e_loop_t *loop;
  E_ALLOC_SIZEOF(loop);
  eloop_init(loop);
  loop->flags |= flags;
  return loop;
}



e_io_t* e_loop_create_tcp_server (e_loop_t* loop, const char* host, int port, e_accept_cb accept_cb){
  e_io_t* io = e_io_create_socket(loop, host, port, EVENT_IO_TYPE_TCP, EVENT_IO_SERVER_SIDE);
//  if (io == NULL) return NULL;
//  hio_setcb_accept(io, accept_cb);
//  hio_accept(io);
//  return io;
}