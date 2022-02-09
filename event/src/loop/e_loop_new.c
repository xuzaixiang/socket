//
// Created by 20123460 on 2022/2/8.
//

#include "../e_event.h"
#include <event/e_loop.h>

static void e_loop_init(e_loop_t *loop) {
#ifdef OS_WIN
  WSAInit();
#endif
#ifdef SIGPIPE
  // NOTE: if not ignore SIGPIPE, write twice when peer close will lead to exit
  // process by SIGPIPE.
  signal(SIGPIPE, SIG_IGN);
#endif
  loop->status = EVENT_LOOP_STATUS_STOP;
  loop->pid = e_get_pid();
  loop->tid = e_get_tid();

  // ios
  io_array_init(&loop->ios, EVENT_IO_ARRAY_INIT_SIZE);

  e_iowatcher_init(loop);

  // custom event
  e_mutex_init(&loop->custom_events_mutex);
  event_queue_init(&loop->custom_events, EVENT_CUSTOM_EVENT_QUEUE_INIT_SIZE);
  EVENT_RESET_LOOP_CUSTOM_FD(loop);
}

e_loop_t *e_loop_new(int flags) {
  e_loop_t *loop;
  EVENT_ALLOC_SIZEOF(loop);
  e_loop_init(loop);
  loop->flags |= flags;
  return loop;
}
