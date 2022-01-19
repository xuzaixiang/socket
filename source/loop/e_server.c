//
// Created by 20123460 on 2022/1/15.
//

#include "event/e_loop.h"
#include "event/e_socket.h"

e_io_t *e_loop_create_tcp_server(e_loop_t *loop, const char *host, int port, e_accept_cb accept_cb) {
  e_io_t *io = e_socket_create(loop, host, port, EVENT_IO_TYPE_TCP, EVENT_IO_SERVER_SIDE);
  if (io == NULL) return NULL;
  e_io_setcb_accept(io, accept_cb);
  e_io_accept(io);
  return io;
}
