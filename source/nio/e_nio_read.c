//
// Created by 20123460 on 2022/1/30.
//
#include "../e_socket.h"
#include "e_nio.h"
#include <event/e_io.h>



static void __read_cb(e_io_t *io, void *buf, int readbytes) {
  // printd("> %.*s\n", readbytes, buf);
  //  io->last_read_hrtime = io->loop->cur_hrtime;
  e_io_handle_read(io, buf, readbytes);
}


static int __nio_read(e_io_t *io, void *buf, int len) {
  int nread = 0;
  switch (io->io_type) {
  case EVENT_IO_TYPE_SSL:
    //      nread = hssl_read(io->ssl, buf, len);
    break;
  case EVENT_IO_TYPE_TCP:
#if defined(EVENT_OS_LINUX) || defined(EVENT_OS_MAC)
    nread = read(io->fd, buf, len);
#else
    nread = recv(io->fd, buf, len, 0);
#endif
    break;
  case EVENT_IO_TYPE_UDP:
  case EVENT_IO_TYPE_KCP:
  case EVENT_IO_TYPE_IP: {
    socklen_t addrlen = sizeof(e_sockaddr_t);
    nread = recvfrom(io->fd, buf, len, 0, io->peeraddr, &addrlen);
  } break;
  default:
    nread = read(io->fd, buf, len);
    break;
  }
  // hlogd("read retval=%d", nread);
  return nread;
}


void e_nio_read(e_io_t *io) {
  // printd("nio_read fd=%d\n", io->fd);
  void *buf;
  int len = 0, nread = 0, err = 0;
read:
  buf = io->readbuf.base + io->readbuf.tail;
  if (io->read_flags & EVENT_IO_READ_UNTIL_LENGTH) {
    len = io->read_until_length - (io->readbuf.tail - io->readbuf.head);
  } else {
    len = io->readbuf.len - io->readbuf.tail;
  }
  assert(len > 0);
  nread = __nio_read(io, buf, len);
  fprintf(stderr, "read retval=%d\n", nread);
  if (nread < 0) {
    err = e_socket_errno();
    if (err == EAGAIN) {
      // goto read_done;
      return;
    } else if (err == EMSGSIZE) {
      // ignore
      return;
    } else {
      io->error = err;
      goto read_error;
    }
  }
  if (nread == 0) {
    goto disconnect;
  }
  io->readbuf.tail += nread;
  __read_cb(io, buf, nread);
  return;
read_error:
disconnect:
  e_io_close(io);
}
