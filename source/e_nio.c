//
// Created by 20123460 on 2022/1/19.
//
#include "event/e_io.h"
#include "e_socket.h"

static void __accept_cb(e_io_t *io) {
  e_io_accept_cb(io);
}

static void nio_read(e_io_t *io) {
  // printd("nio_read fd=%d\n", io->fd);
  void *buf;
  int len = 0, nread = 0, err = 0;
  read:
//  buf = io->readbuf.base + io->readbuf.tail;
//  if (io->read_flags & HIO_READ_UNTIL_LENGTH) {
//    len = io->read_until_length - (io->readbuf.tail - io->readbuf.head);
//  } else {
//    len = io->readbuf.len - io->readbuf.tail;
//  }
//  assert(len > 0);
//  nread = __nio_read(io, buf, len);
//  // printd("read retval=%d\n", nread);
//  if (nread < 0) {
//    err = e_socket_errno();
//    if (err == EAGAIN) {
//      // goto read_done;
//      return;
//    } else if (err == EMSGSIZE) {
//      // ignore
//      return;
//    } else {
//      // perror("read");
//      io->error = err;
//      goto read_error;
//    }
//  }
//  if (nread == 0) {
//    goto disconnect;
//  }
//  io->readbuf.tail += nread;
//  __read_cb(io, buf, nread);
  // if (nread == len) goto read;
  return;
  read_error:
  disconnect:
  e_io_close(io);
}

static void nio_accept(e_io_t *io) {
  // printd("nio_accept listenfd=%d\n", io->fd);
  int connfd = 0, err = 0, accept_cnt = 0;
  socklen_t addrlen;
  e_io_t *connio = NULL;
  while (accept_cnt++ < 3) {
    addrlen = sizeof(e_sockaddr_t);
    connfd = accept(io->fd, io->peeraddr, &addrlen);
    if (connfd < 0) {
      err = e_socket_errno();
      if (err == EAGAIN || err == EINTR) {
        return;
      } else {
        perror("accept");
        io->error = err;
        goto accept_error;
      }
    }
    addrlen = sizeof(e_sockaddr_t);
    getsockname(connfd, io->localaddr, &addrlen);
    connio = e_io_get(io->loop, connfd);
    // NOTE: inherit from listenio
    connio->accept_cb = io->accept_cb;
    connio->userdata = io->userdata;
//    if (io->unpack_setting) {
//      e_io_set_unpack(connio, io->unpack_setting);
//    }
//
    if (io->io_type == EVENT_IO_TYPE_SSL) {
//      if (connio->ssl == NULL) {
//        hssl_ctx_t ssl_ctx = hssl_ctx_instance();
//        if (ssl_ctx == NULL) {
//          io->error = HSSL_ERROR;
//          goto accept_error;
//        }
//        hssl_t ssl = hssl_new(ssl_ctx, connfd);
//        if (ssl == NULL) {
//          io->error = HSSL_ERROR;
//          goto accept_error;
//        }
//        connio->ssl = ssl;
//      }
//      hio_enable_ssl(connio);
//      ssl_server_handshake(connio);
    } else {
//      // NOTE: SSL call accept_cb after handshake finished
      __accept_cb(connio);
    }
  }
  return;

  accept_error:
//  hloge("listenfd=%d accept error: %s:%d", io->fd, socket_strerror(io->error), io->error);
  e_io_close(io);
}

static void e_io_handle_events(e_io_t *io) {
  if ((io->events & EVENT_READ) && (io->revents & EVENT_READ)) {
    if (io->accept) {
      nio_accept(io);
    } else {
//      nio_read(io);
    }
  }

  if ((io->events & EVENT_WRITE) && (io->revents & EVENT_WRITE)) {
//    // NOTE: del HV_WRITE, if write_queue empty
//    hrecursive_mutex_lock(&io->write_mutex);
//    if (write_queue_empty(&io->write_queue)) {
//      iowatcher_del_event(io->loop, io->fd, HV_WRITE);
//      io->events &= ~HV_WRITE;
//    }
//    hrecursive_mutex_unlock(&io->write_mutex);
//    if (io->connect) {
//      // NOTE: connect just do once
//      // ONESHOT
//      io->connect = 0;
//
//      nio_connect(io);
//    }
//    else {
//      nio_write(io);
//    }
  }

  io->revents = 0;
}

int e_io_accept(e_io_t *io) {
  io->accept = 1;
  e_io_add(io, e_io_handle_events, EVENT_READ);
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

int e_io_close(e_io_t *io) {

}
