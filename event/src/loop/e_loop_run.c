//
// Created by 20123460 on 2022/2/9.
//
#include "../e_event.h"

int e_loop_run(e_loop_t *loop) {
  if (loop == NULL)
    return -1;
  if (loop->status == EVENT_LOOP_STATUS_RUNNING)
    return -2;
  loop->status = EVENT_LOOP_STATUS_RUNNING;
  loop->pid = e_get_pid();
  loop->tid = e_get_tid();

  while (loop->status != EVENT_LOOP_STATUS_STOP) {
    if (loop->status == EVENT_LOOP_STATUS_PAUSE) {
      e_mutex_lock(&loop->loop_mutex);
      e_cond_wait(&loop->loop_cond, &loop->loop_mutex);
      e_mutex_unlock(&loop->loop_mutex);
    }
    if ((loop->flags & EVENT_LOOP_FLAG_QUIT_WHEN_NO_ACTIVE_EVENTS) &&
        loop->nactives <= 1) {
      break;
    }
    e_loop_handle(loop);
    if (loop->flags & EVENT_LOOP_FLAG_RUN_ONCE) {
      break;
    }
  }
  loop->status = EVENT_LOOP_STATUS_STOP;
  if (loop->flags & EVENT_LOOP_FLAG_AUTO_FREE) {
    e_loop_free(&loop);
  }
  return 0;
}