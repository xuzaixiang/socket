//
// Created by 20123460 on 2022/2/9.
//

#include "e_event.h"

void e_io_free(e_io_t *io) {}

void e_io_init(e_io_t *io) {
  //  hrecursive_mutex_init(&io->write_mutex);
}
void e_io_ready(e_io_t *io) {
  if (io->ready)
    return;
  // flags
  io->ready = 1;
  io->closed = 0;
}

e_io_t *e_io_get(e_loop_t *loop, int fd) {
  if (fd > io_array_capacity(&loop->ios)) {
    io_array_resize(&loop->ios, fd);
  }
  e_io_t *io = *io_array_get(&loop->ios, fd);
  if (io == NULL) {
    EVENT_ALLOC_SIZEOF(io);
    e_io_init(io);
    io->event_type = EVENT_TYPE_IO;
    io->loop = loop;
    io->fd = fd;
    io_array_set(&loop->ios,fd,io);
  }
  if (!io->ready) {
    e_io_ready(io);
  }
  return io;
}