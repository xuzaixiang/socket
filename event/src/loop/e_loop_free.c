//
// Created by 20123460 on 2022/2/9.
//

#include "../e_event.h"

static void e_loop_cleanup(e_loop_t *loop) {
  // ios
  for (int i = 0; i < io_array_size(&loop->ios); ++i) {
    e_io_t *io = *io_array_get(&loop->ios, i);
    if (io) {
      e_io_free(io);
    }
  }
  io_array_free(&loop->ios);

  // iowatcher
  e_iowatcher_cleanup(loop);

  // custom event
  e_mutex_lock(&loop->custom_events_mutex);
  event_queue_cleanup(&loop->custom_events);
  e_loop_close_custom_fd(loop);
  e_mutex_unlock(&loop->custom_events_mutex);
  e_mutex_destroy(&loop->custom_events_mutex);

  // loop status
  e_mutex_destroy(&loop->loop_mutex);
  e_cond_destroy(&loop->loop_cond);
}

void e_loop_free(e_loop_t **pp) {
  if (pp && *pp) {
    e_loop_cleanup(*pp);
    EVENT_FREE(*pp);
    *pp = NULL;
  }
}
