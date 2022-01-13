
#include "event/e_io.h"

e_accept_cb e_io_getcb_accept(e_io_t *io){
  return io->accept_cb;
}
e_connect_cb e_io_getcb_connect(e_io_t *io){
  return io->connect_cb;
}
e_read_cb e_io_getcb_read(e_io_t *io){
  return io->read_cb;
}
e_write_cb e_io_getcb_write(e_io_t *io){
  return io->write_cb;
}
e_close_cb e_io_getcb_close(e_io_t *io){
  return io->close_cb;
}


void e_io_setcb_accept(e_io_t *io, e_accept_cb accept_cb) {
  io->accept_cb = accept_cb;
}
void e_io_setcb_close(e_io_t *io, e_close_cb close_cb) {
  io->close_cb = close_cb;
}
void e_io_setcb_connect(e_io_t *io, e_connect_cb connect_cb) {
  io->connect_cb = connect_cb;
}
void e_io_setcb_read(e_io_t *io, e_read_cb read_cb) {
  io->read_cb = read_cb;
}
void e_io_setcb_write(e_io_t *io, e_write_cb write_cb) {
  io->write_cb = write_cb;
}


void e_io_accept_cb(e_io_t *io) {
  /*
   char localaddrstr[SOCKADDR_STRLEN] = {0};
   char peeraddrstr[SOCKADDR_STRLEN] = {0};
   printd("accept connfd=%d [%s] <= [%s]\n", io->fd,
           SOCKADDR_STR(io->localaddr, localaddrstr),
           SOCKADDR_STR(io->peeraddr, peeraddrstr));
   */
  if (io->accept_cb) {
    // printd("accept_cb------\n");
    io->accept_cb(io);
    // printd("accept_cb======\n");
  }
}

void e_io_connect_cb(e_io_t* io){
  /*
 char localaddrstr[SOCKADDR_STRLEN] = {0};
 char peeraddrstr[SOCKADDR_STRLEN] = {0};
 printd("connect connfd=%d [%s] => [%s]\n", io->fd,
         SOCKADDR_STR(io->localaddr, localaddrstr),
         SOCKADDR_STR(io->peeraddr, peeraddrstr));
 */
  if (io->connect_cb) {
    // printd("connect_cb------\n");
    io->connect_cb(io);
    // printd("connect_cb======\n");
  }
}


void e_io_handle_read(e_io_t* io, void* buf, int readbytes) {
#if WITH_KCP
  if (io->io_type == HIO_TYPE_KCP) {
        hio_read_kcp(io, buf, readbytes);
        return;
    }
#endif

  if (io->unpack_setting) {
    // hio_set_unpack
    e_io_unpack(io, buf, readbytes);
  } else {
    const unsigned char* sp = (const unsigned char*)io->readbuf.base + io->readbuf.head;
    const unsigned char* ep = (const unsigned char*)buf + readbytes;
    if (io->read_flags & EVENT_IO_READ_UNTIL_LENGTH) {
      // hio_read_until_length
      if (ep - sp >= io->read_until_length) {
        io->readbuf.head += io->read_until_length;
        if (io->readbuf.head == io->readbuf.tail) {
          io->readbuf.head = io->readbuf.tail = 0;
        }
        io->read_flags &= ~EVENT_IO_READ_UNTIL_LENGTH;
        e_io_read_cb(io, (void*)sp, io->read_until_length);
      }
    } else if (io->read_flags & EVENT_IO_READ_UNTIL_DELIM) {
      // hio_read_until_delim
      const unsigned char* p = (const unsigned char*)buf;
      for (int i = 0; i < readbytes; ++i, ++p) {
        if (*p == io->read_until_delim) {
          int len = p - sp + 1;
          io->readbuf.head += len;
          if (io->readbuf.head == io->readbuf.tail) {
            io->readbuf.head = io->readbuf.tail = 0;
          }
          io->read_flags &= ~EVENT_IO_READ_UNTIL_DELIM;
          e_io_read_cb(io, (void*)sp, len);
          return;
        }
      }
    } else {
      // hio_read
      io->readbuf.head = io->readbuf.tail = 0;
      e_io_read_cb(io, (void*)sp, ep - sp);
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
      memmove(io->readbuf.base, io->readbuf.base + io->readbuf.head, io->readbuf.tail - io->readbuf.head);
    }
  } else {
    size_t small_size = io->readbuf.len / 2;
    if (io->readbuf.tail < small_size &&
        io->small_readbytes_cnt >= 3) {
      // scale down / 2
      e_io_alloc_readbuf(io, small_size);
    }
  }
}

void e_io_read_cb(e_io_t *io, void *buf, int len) {
  if (io->read_flags & EVENT_IO_READ_ONCE) {
    io->read_flags &= ~EVENT_IO_READ_ONCE;
    e_io_read_stop(io);
  }

  if (io->read_cb) {
    // printd("read_cb------\n");
    io->read_cb(io, buf, len);
    // printd("read_cb======\n");
  }

  // for readbuf autosize
  if (e_io_is_alloced_readbuf(io) && io->readbuf.len > EVENT_READ_BUFSIZE_HIGH_WATER) {
    size_t small_size = io->readbuf.len / 2;
    if (len < small_size) {
      ++io->small_readbytes_cnt;
    } else {
      io->small_readbytes_cnt = 0;
    }
  }
}

void e_io_write_cb(e_io_t *io, const void *buf, int len) {
  if (io->write_cb) {
    // printd("write_cb------\n");
    io->write_cb(io, buf, len);
    // printd("write_cb======\n");
  }
}

void e_io_close_cb(e_io_t *io) {
  if (io->close_cb) {
    // printd("close_cb------\n");
    io->close_cb(io);
    // printd("close_cb======\n");
  }
}

