//
// Created by 20123460 on 2022/1/19.
//
#include "event/e_io.h"
#include "event/e_socket.h"

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
//    if (io->io_type == HIO_TYPE_SSL) {
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
//    } else {
//      // NOTE: SSL call accept_cb after handshake finished
//      __accept_cb(connio);
//    }
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

//  if ((io->events & HV_WRITE) && (io->revents & HV_WRITE)) {
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
//  }
//
//  io->revents = 0;
}

int e_io_accept(e_io_t *io) {
  io->accept = 1;
  e_io_add(io, e_io_handle_events, EVENT_READ);
  return 0;
}

int e_io_close(e_io_t *io){

}
