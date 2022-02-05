//
// Created by 20123460 on 2022/1/30.
//

#include "../e_socket.h"
#include "../nio/e_nio.h"

void e_io_handle_events(e_io_t *io) {
  if ((io->events & EVENT_READ) && (io->revents & EVENT_READ)) {
    if (io->accept) {
      e_nio_accept(io);
    } else {
      e_nio_read(io);
    }
  }

  if ((io->events & EVENT_WRITE) && (io->revents & EVENT_WRITE)) {
    // NOTE: del HV_WRITE, if write_queue empty
    e_recursive_mutex_lock(&io->write_mutex);
    if (write_queue_empty(&io->write_queue)) {
      e_iowatcher_del_event(io->loop, io->fd, EVENT_WRITE);
      io->events &= ~EVENT_WRITE;
    }
    e_recursive_mutex_unlock(&io->write_mutex);
    if (io->connect) {
      // NOTE: connect just do once
      // ONESHOT
      io->connect = 0;

      e_nio_connect(io);
    } else {
      e_nio_write(io);
    }
  }

  io->revents = 0;
}
