//
// Created by 20123460 on 2022/2/4.
//

#include "e_io_handle.h"
#include "e_io_readbuf.h"

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
  if (io->readbuf.len > EVENT_READ_BUFSIZE_HIGH_WATER) {
    size_t small_size = io->readbuf.len / 2;
    if (len < small_size) {
      ++io->small_readbytes_cnt;
    } else {
      io->small_readbytes_cnt = 0;
    }
  }
}

int e_io_read(e_io_t *io) {
  if (io->closed) {
    fprintf(stderr, "e_io_read called but fd[%d] already closed!", io->fd);
    return -1;
  }
  e_io_add(io, e_io_handle_events, EVENT_READ);
  if (io->readbuf.tail > io->readbuf.head && io->unpack_setting == NULL &&
      io->read_flags == 0) {
    e_io_read_remain(io);
  }
  return 0;
}

int e_io_read_once(e_io_t *io) {
  io->read_flags |= EVENT_IO_READ_ONCE;
  return e_io_read_start(io);
}

int e_io_read_until_length(e_io_t *io, unsigned int len) {
  if (len == 0)
    return 0;
  if (io->readbuf.tail - io->readbuf.head >= len) {
    void *buf = io->readbuf.base + io->readbuf.head;
    io->readbuf.head += len;
    if (io->readbuf.head == io->readbuf.tail) {
      io->readbuf.head = io->readbuf.tail = 0;
    }
    e_io_read_cb(io, buf, len);
    return len;
  }
  io->read_flags = EVENT_IO_READ_UNTIL_LENGTH;
  io->read_until_length = len;
  // NOTE: prepare readbuf
  if (e_io_is_loop_readbuf(io) || io->readbuf.len < len) {
    e_io_alloc_readbuf(io, len);
  }
  return e_io_read_once(io);
}

int e_io_read_until_delim(e_io_t *io, unsigned char delim) {
  if (io->readbuf.tail - io->readbuf.head > 0) {
    const unsigned char *sp =
        (const unsigned char *)io->readbuf.base + io->readbuf.head;
    const unsigned char *ep =
        (const unsigned char *)io->readbuf.base + io->readbuf.tail;
    const unsigned char *p = sp;
    while (p <= ep) {
      if (*p == delim) {
        int len = p - sp + 1;
        io->readbuf.head += len;
        if (io->readbuf.head == io->readbuf.tail) {
          io->readbuf.head = io->readbuf.tail = 0;
        }
        e_io_read_cb(io, (void *)sp, len);
        return len;
      }
      ++p;
    }
  }
  io->read_flags = EVENT_IO_READ_UNTIL_DELIM;
  io->read_until_length = delim;
  // NOTE: prepare readbuf
  if (e_io_is_loop_readbuf(io) || io->readbuf.len < EVENT_LOOP_READ_BUFSIZE) {
    e_io_alloc_readbuf(io, EVENT_LOOP_READ_BUFSIZE);
  }
  return e_io_read_once(io);
}

int e_io_read_remain(e_io_t *io) {
  int remain = io->readbuf.tail - io->readbuf.head;
  if (remain > 0) {
    void *buf = io->readbuf.base + io->readbuf.head;
    io->readbuf.head = io->readbuf.tail = 0;
    e_io_read_cb(io, buf, remain);
  }
  return remain;
}