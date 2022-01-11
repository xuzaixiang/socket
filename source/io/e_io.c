
#include "event/io/e_io.h"
#include "event/io/e_socket.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

e_io_t *e_io_create_socket(e_loop_t *loop, const char *host, int port,
                           e_io_type_e type DEFAULT(EVENT_IO_TYPE_TCP),
                           e_io_side_e side DEFAULT(EVENT_IO_SERVER_SIDE)){
  int sock_type = type & EVENT_IO_TYPE_SOCK_STREAM ? SOCK_STREAM :
                  type & EVENT_IO_TYPE_SOCK_DGRAM  ? SOCK_DGRAM :
                  type & EVENT_IO_TYPE_SOCK_RAW    ? SOCK_RAW : -1;
  if (sock_type == -1) return NULL;
  sockaddr_u addr;
  memset(&addr, 0, sizeof(addr));
  int ret = -1;
//#ifdef ENABLE_UDS
//  if (port < 0) {
//        sockaddr_set_path(&addr, host);
//        ret = 0;
//    }
//#endif
  if (port >= 0) {
    ret = sockaddr_set_ipport(&addr, host, port);
  }
  if (ret != 0) {
    // fprintf(stderr, "unknown host: %s\n", host);
    return NULL;
  }
  int sockfd = socket(addr.sa.sa_family, sock_type, 0);
  if (sockfd < 0) {
    perror("socket");
    return NULL;
  }
  e_io_t* io = NULL;
  if (side == EVENT_IO_SERVER_SIDE) {
//#ifdef SO_REUSEADDR
//    // NOTE: SO_REUSEADDR allow to reuse sockaddr of TIME_WAIT status
//    int reuseaddr = 1;
//    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuseaddr, sizeof(int)) < 0) {
//      perror("setsockopt");
//      closesocket(sockfd);
//      return NULL;
//    }
//#endif
//    if (bind(sockfd, &addr.sa, sockaddr_len(&addr)) < 0) {
//      perror("bind");
//      closesocket(sockfd);
//      return NULL;
//    }
//    if (sock_type == SOCK_STREAM) {
//      if (listen(sockfd, SOMAXCONN) < 0) {
//        perror("listen");
//        closesocket(sockfd);
//        return NULL;
//      }
//    }
//  }
//  io = hio_get(loop, sockfd);
//  assert(io != NULL);
//  io->io_type = type;
//  if (side == EVENT_IO_SERVER_SIDE) {
//    hio_set_localaddr(io, &addr.sa, sockaddr_len(&addr));
//    io->priority = HEVENT_HIGH_PRIORITY;
//  } else {
//    hio_set_peeraddr(io, &addr.sa, sockaddr_len(&addr));
  }
  return io;
}