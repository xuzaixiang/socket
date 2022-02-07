//
// Created by 20123460 on 2022/1/19.
//
#include "e_nio.h"
#include "event/e_io.h"
#include "event/e_loop.h"

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

void e_io_connect_cb(e_io_t *io) {
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

void e_io_write_cb(e_io_t *io, const void *buf, int len) {
  if (io->write_cb) {
    // printd("write_cb------\n");
    io->write_cb(io, buf, len);
    // printd("write_cb======\n");
  }
}

