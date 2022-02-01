//
// Created by 20123460 on 2022/1/19.
//

#include "../e_socket.h"
#include "e_nio_handle.h"
#include "event/e_io.h"
#include "event/e_loop.h"

int e_io_accept(e_io_t *io) {
  io->accept = 1;
  e_io_add(io, e_io_handle_events, EVENT_READ);
  return 0;
}

int e_io_del(e_io_t *io, int events DEFAULT(EVENT_RDWR)) {
  if (!io->active)
    return -1;

  if (io->events & events) {
    e_iowatcher_del_event(io->loop, io->fd, events);
    io->events &= ~events;
  }
  if (io->events == 0) {
    io->loop->nios--;
    // NOTE: not EVENT_DEL, avoid free
    EVENT_INACTIVE(io);
  }
  return 0;
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
  if (e_io_is_alloced_readbuf(io) &&
      io->readbuf.len > EVENT_READ_BUFSIZE_HIGH_WATER) {
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

int e_io_close(e_io_t *io) {}