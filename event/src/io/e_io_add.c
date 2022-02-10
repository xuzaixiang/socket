//
// Created by 20123460 on 2022/2/10.
//
#include "../e_event.h"

int e_io_add(e_io_t *io, e_io_cb cb, int events) {
#ifdef EVENT_OS_WIN
  // Windows iowatcher not work on stdio
  if (io->fd < 3)
    return -1;
#endif
  e_loop_t *loop = io->loop;
  if (!io->active) {
    EVENT_ADD(loop, io, cb);
    loop->nios++;
  }
  if (!io->ready) {
    e_io_ready(io);
  }
  if (cb) {
    io->cb = (e_event_cb)cb;
  }
  if (!(io->events & events)) {
    e_iowatcher_add_event(loop, io->fd, events);
    io->events |= events;
  }
  return 0;
}