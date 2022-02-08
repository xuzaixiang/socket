//
// Created by 20123460 on 2022/2/4.
//

#include "e_io_handle.h"
#include <event/e_loop.h>

int e_io_del(e_io_t *io, int events DEFAULT(EVENT_RDWR)) {
#ifdef EVENT_OS_WIN
  // Windows iowatcher not work on stdio
  if (io->fd < 3) return -1;
#endif
  if (!io->active)
    return -1;

  if (io->events & events) {
    e_iowatcher_del_event(io->loop, io->fd, events);
    io->events &= ~events;
  }
  if (io->events == 0) {
    io->loop->nios--;
    // NOTE: not EVENT_DEL, avoid free
    EVENT_INACTIVE(io);
  }
  return 0;
}
