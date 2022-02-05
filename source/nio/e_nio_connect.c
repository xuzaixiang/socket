//
// Created by 20123460 on 2022/2/2.
//

#include "e_nio.h"

static void __connect_cb(e_io_t *io) {
  //  e_io_del_connect_timer(io);
  e_io_connect_cb(io);
}

void e_nio_connect(e_io_t *io) {
  // printd("nio_connect connfd=%d\n", io->fd);
  socklen_t addrlen = sizeof(e_sockaddr_t);
  int ret = getpeername(io->fd, io->peeraddr, &addrlen);
  if (ret < 0) {
    io->error = e_socket_errno();
    fprintf(stderr, "connect failed: %s: %d\n", strerror(io->error), io->error);
    goto connect_failed;
  } else {
    addrlen = sizeof(e_sockaddr_t);
    getsockname(io->fd, io->localaddr, &addrlen);

    if (io->io_type == EVENT_IO_TYPE_SSL) {
      //      if (io->ssl == NULL) {
      //        // io->ssl_ctx > g_ssl_ctx > hssl_ctx_new
      //        hssl_ctx_t ssl_ctx = NULL;
      //        if (io->ssl_ctx) {
      //          ssl_ctx = io->ssl_ctx;
      //        } else if (g_ssl_ctx) {
      //          ssl_ctx = g_ssl_ctx;
      //        } else {
      //          io->ssl_ctx = ssl_ctx = hssl_ctx_new(NULL);
      //          io->alloced_ssl_ctx = 1;
      //        }
      //        if (ssl_ctx == NULL) {
      //          goto connect_failed;
      //        }
      //        hssl_t ssl = hssl_new(ssl_ctx, io->fd);
      //        if (ssl == NULL) {
      //          goto connect_failed;
      //        }
      //        io->ssl = ssl;
      //      }
      //      ssl_client_handshake(io);
    } else {
      // NOTE: SSL call connect_cb after handshake finished
      __connect_cb(io);
    }

    return;
  }

connect_failed:
  e_io_close(io);
}
