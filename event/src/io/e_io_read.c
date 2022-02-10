//
// Created by 20123460 on 2022/2/10.
//
#include "../e_event.h"

int e_io_read(e_io_t *io, e_read_cb read_cb) {
  if (io->closed)
    return -1;
  if (read_cb) {
    io->read_cb = read_cb;
  }
  return e_io_add(io, e_io_handle, EVENT_READ);
}