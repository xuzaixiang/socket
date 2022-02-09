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

  return 0;
}