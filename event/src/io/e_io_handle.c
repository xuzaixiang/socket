//
// Created by 20123460 on 2022/2/10.
//

#include "../e_event.h"

void e_io_handle(e_io_t *io) {
  if ((io->events & EVENT_READ) && (io->revents & EVENT_READ)) {
    if (io->read_cb) {
      io->read_cb(io);
    }
  }
  if ((io->events & EVENT_WRITE) && (io->revents & EVENT_WRITE)) {
    // NOTE: del HV_WRITE, if write_queue empty
    //    hrecursive_mutex_lock(&io->write_mutex);
    //    if (write_queue_empty(&io->write_queue)) {
    //      iowatcher_del_event(io->loop, io->fd, HV_WRITE);
    //      io->events &= ~HV_WRITE;
    //    }
    //    hrecursive_mutex_unlock(&io->write_mutex);
    //    nio_write(io);
    if (io->write_cb) {
      io->write_cb(io);
    }
  }
  io->revents = 0;
}