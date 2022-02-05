//
// Created by 20123460 on 2022/2/4.
//

#include "e_io_private.h"

int e_io_read(e_io_t *io) {
  if (io->closed) {
    fprintf(stderr, "e_io_read called but fd[%d] already closed!", io->fd);
    return -1;
  }
  e_io_add(io, e_io_handle_events, EVENT_READ);
  //  if (io->readbuf.tail > io->readbuf.head && io->unpack_setting == NULL &&
  //      io->read_flags == 0) {
  //    hio_read_remain(io);
  //  }
  return 0;
}
