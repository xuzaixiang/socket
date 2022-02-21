//
// Created by 20123460 on 2022/2/10.
//
#include "e_event.h"

void e_loop_check_io_size(e_loop_t *loop, int fd) {
  //  if (fd >= loop->ios.maxsize) {
  //    int newsize = (int)ceil2e(fd);
  //    io_array_resize(&loop->ios, newsize > fd ? newsize : 2 * fd);
  //  }
}

void e_loop_handle(e_loop_t *loop) {
  e_iowatcher_poll_events(loop, EVENT_IOWATCHER_BLOCK_TIME_INDEFINITELY);
  if (loop->npendings == 0)
    return;

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
}

int e_loop_pause(e_loop_t *loop) {
  if (loop->status == EVENT_LOOP_STATUS_PAUSE)
    return 0;
  if (loop->status == EVENT_LOOP_STATUS_STOP)
    return -1;
  loop->status = EVENT_LOOP_STATUS_PAUSE;
  e_event_t ev;
  memset(&ev, 0, sizeof(ev));
  e_loop_post(loop, &ev);
  return 0;
}

int e_loop_resume(e_loop_t *loop) {

  return 0;
}