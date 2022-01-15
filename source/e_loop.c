//
// Created by 20123460 on 2022/1/13.
//

#include "event/e_loop.h"

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
