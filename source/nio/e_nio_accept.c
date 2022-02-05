//
// Created by 20123460 on 2022/2/2.
//

#include "e_nio.h"


static void __accept_cb(e_io_t *io) { e_io_accept_cb(io); }


void e_nio_accept(e_io_t *io) {
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
  //  hloge("listenfd=%d accept error: %s:%d", io->fd,
  //  socket_strerror(io->error), io->error);
  e_io_close(io);
}



int e_io_accept(e_io_t *io) {
  io->accept = 1;
  e_io_add(io, e_io_handle_events, EVENT_READ);
  return 0;
}
