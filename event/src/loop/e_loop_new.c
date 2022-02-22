//
// Created by 20123460 on 2022/2/8.
//

#include "../e_event.h"
#include <event/e_loop.h>

static int e_loop_init(e_loop_t *loop) {
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

  /// ios
  loop->ios = stc_vector_new_of_io(0);
  stc_vector_resize(loop->ios,EVENT_IO_ARRAY_INIT_SIZE);

  /// iowatcher
  e_iowatcher_init(loop);

  /// loop status
  e_mutex_init(&loop->loop_mutex);
  e_cond_init(&loop->loop_cond);

  /// create custom event
  e_mutex_init(&loop->custom_events_mutex);
  event_queue_init(&loop->custom_events, EVENT_CUSTOM_EVENT_QUEUE_INIT_SIZE);
  if (e_loop_create_custom_fd(loop) < 0) {
    return -1;
  }
  /// read custom fd
  e_io_t *io = e_io_get(loop, e_loop_get_custom_read_fd(loop));
  if (io == NULL)
    return -1;
  e_io_read(io, e_loop_handle_custom_event);
  io->priority = EVENT_HIGH_PRIORITY;
  return 0;
}

e_loop_t *e_loop_new(int flags) {
  e_loop_t *loop;
  EVENT_ALLOC_SIZEOF(loop);
  if (e_loop_init(loop) < 0) {
    EVENT_FREE(loop);
    return NULL;
  }
  loop->flags |= flags;
  return loop;
}
