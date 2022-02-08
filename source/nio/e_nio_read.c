//
// Created by 20123460 on 2022/1/30.
//
#include "../e_socket.h"
#include "../e_include.h"
#include "e_nio.h"
#include <event/e_io.h>

static void nio_read_handle(e_io_t *io, void *buf, int readbytes) {
#if WITH_KCP
  if (io->io_type == HIO_TYPE_KCP) {
    hio_read_kcp(io, buf, readbytes);
    return;
  }
#endif

  if (io->unpack_setting) {
    e_io_unpack(io, buf, readbytes);
  } else {
    const unsigned char *sp =
        (const unsigned char *)io->readbuf.base + io->readbuf.head;
    const unsigned char *ep = (const unsigned char *)buf + readbytes;
    if (io->read_flags & EVENT_IO_READ_UNTIL_LENGTH) {
      // e_io_read_until_length
      if (ep - sp >= io->read_until_length) {
        io->readbuf.head += io->read_until_length;
        if (io->readbuf.head == io->readbuf.tail) {
          io->readbuf.head = io->readbuf.tail = 0;
        }
        io->read_flags &= ~EVENT_IO_READ_UNTIL_LENGTH;
        e_io_read_cb(io, (void *)sp, io->read_until_length);
      }
    } else if (io->read_flags & EVENT_IO_READ_UNTIL_DELIM) {
      // e_io_read_until_delim
      const unsigned char *p = (const unsigned char *)buf;
      for (int i = 0; i < readbytes; ++i, ++p) {
        if (*p == io->read_until_delim) {
          int len = p - sp + 1;
          io->readbuf.head += len;
          if (io->readbuf.head == io->readbuf.tail) {
            io->readbuf.head = io->readbuf.tail = 0;
          }
          io->read_flags &= ~EVENT_IO_READ_UNTIL_DELIM;
          e_io_read_cb(io, (void *)sp, len);
          return;
        }
      }
    } else {
      // e_io_read
      io->readbuf.head = io->readbuf.tail = 0;
      e_io_read_cb(io, (void *)sp, ep - sp);
    }
  }

  if (io->readbuf.head == io->readbuf.tail) {
    io->readbuf.head = io->readbuf.tail = 0;
  }
  // readbuf autosize
  if (io->readbuf.tail == io->readbuf.len) {
    if (io->readbuf.head == 0) {
      // scale up * 2
      e_io_alloc_readbuf(io, io->readbuf.len * 2);
    } else {
      // [head, tail] => [base, tail - head]
      memmove(io->readbuf.base, io->readbuf.base + io->readbuf.head,
              io->readbuf.tail - io->readbuf.head);
    }
  } else {
    size_t small_size = io->readbuf.len / 2;
    if (io->readbuf.tail < small_size && io->small_readbytes_cnt >= 3) {
      // scale down / 2
      e_io_alloc_readbuf(io, small_size);
    }
  }
}

static void nio_read_cb(e_io_t *io, void *buf, int readbytes) {
  // printd("> %.*s\n", readbytes, buf);
  // todo
  //  io->last_read_hrtime = io->loop->cur_hrtime;
  nio_read_handle(io, buf, readbytes);
}

static int nio_read(e_io_t *io, void *buf, int len) {
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
  void *buf;
  int len, nread;

  //                   head               tail ----------- buf ----------|
  //  |                 |                  |                             |
  //  --------------------------------------------------------------------
  //  {$link EVENT_IO_READ_UNTIL_LENGTH}
  //  |                 |---- read until length ----------|              |
  //  |                                    |----- len ----|              |
  //  {$link EVENT_IO_READ_ONCE}
  //  {$link EVENT_IO_READ_UNTIL_DELIM}
  //  |                                    |------------ len ------------|

  buf = io->readbuf.base + io->readbuf.tail;
  if (io->read_flags & EVENT_IO_READ_UNTIL_LENGTH) {
    len = io->read_until_length - (io->readbuf.tail - io->readbuf.head);
  } else {
    len = io->readbuf.len - io->readbuf.tail;
  }
  assert(len > 0);
  nread = nio_read(io, buf, len);
  fprintf(stderr, "read retval=%d\n", nread);
  if (nread < 0) {
    int err = e_socket_errno();
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
  nio_read_cb(io, buf, nread);
  return;
read_error:
disconnect:
  e_io_close(io);
}
