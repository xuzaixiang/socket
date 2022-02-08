//
// Created by 20123460 on 2022/2/8.
//
#include <event/e_io.h>
#include <event/e_loop.h>
#include "../util/e_math.h"

e_io_t *e_io_get(e_loop_t *loop, int fd) {
  if (fd >= loop->ios.maxsize) {
    int newsize = ceil2e(fd);
    io_array_resize(&loop->ios, newsize > fd ? newsize : 2 * fd);
  }
  e_io_t *io = loop->ios.ptr[fd];
  if (io == NULL) {
    EVENT_ALLOC_SIZEOF(io);
    e_io_init(io);
    io->event_type = EVENT_TYPE_IO;
    io->loop = loop;
    io->fd = fd;
    loop->ios.ptr[fd] = io;
  }
  if (!io->ready) {
    e_io_ready(io);
  }

  return io;
}
