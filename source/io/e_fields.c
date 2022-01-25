//
// Created by 20123460 on 2022/1/19.
//

#include "event/e_io.h"

void e_io_setcb_accept(e_io_t *io, e_accept_cb accept_cb) {
  io->accept_cb = accept_cb;
}
void e_io_setcb_read(e_io_t *io, e_read_cb read_cb) {
  io->read_cb = read_cb;
}

int e_io_fd(e_io_t *io) {
  return io->fd;
}
struct sockaddr *e_io_localaddr(e_io_t *io) {
  return io->localaddr;
}
struct sockaddr *e_io_peeraddr(e_io_t *io) {
  return io->peeraddr;
}

void e_io_set_localaddr(e_io_t *io, struct sockaddr *addr, int addrlen) {
  if (io->localaddr == NULL) {
    EVENT_ALLOC(io->localaddr, sizeof(e_sockaddr_t));
  }
  memcpy(io->localaddr, addr, addrlen);
}
void e_io_set_peeraddr(e_io_t *io, struct sockaddr *addr, int addrlen) {
  if (io->peeraddr == NULL) {
    EVENT_ALLOC(io->peeraddr, sizeof(e_sockaddr_t));
  }
  memcpy(io->peeraddr, addr, addrlen);
}
