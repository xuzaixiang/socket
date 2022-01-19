//
// Created by 20123460 on 2022/1/15.
//

#include "event/e_socket.h"
#include "event/e_sockaddr.h"
#include <errno.h>

int e_socket_errno(){
#ifdef EVENT_OS_WIN
  return WSAGetLastError();
#endif
  return errno;
}

e_io_t *e_socket_create(e_loop_t *loop, const char *host, int port, e_io_type_t type, e_io_side_t side) {
  int sock_type = type & EVENT_IO_TYPE_SOCK_STREAM ? SOCK_STREAM :
                  type & EVENT_IO_TYPE_SOCK_DGRAM ? SOCK_DGRAM :
                  type & EVENT_IO_TYPE_SOCK_RAW ? SOCK_RAW : -1;
  if (sock_type == -1) return NULL;
  e_sockaddr_t addr;
  memset(&addr, 0, sizeof(addr));
  int ret = -1;
  if (port >= 0) {
    ret = e_sockaddr_set_ipport(&addr, host, port);
  }
  if (ret != 0) {
    fprintf(stderr, "unknown host: %s\n", host);
    return NULL;
  }
  int sockfd = socket(addr.sa.sa_family, sock_type, 0);
  if (sockfd < 0) {
    perror("socket");
    return NULL;
  }
  e_io_t *io = NULL;
  if (side == EVENT_IO_SERVER_SIDE) {
#ifdef SO_REUSEADDR
    // NOTE: SO_REUSEADDR allow to reuse sockaddr of TIME_WAIT status
    int reuseaddr = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *) &reuseaddr, sizeof(int)) < 0) {
      perror("setsockopt");
      closesocket(sockfd);
      return NULL;
    }
#endif
    if (bind(sockfd, &addr.sa, e_sockaddr_len(&addr)) < 0) {
      perror("bind");
      closesocket(sockfd);
      return NULL;
    }
    if (sock_type == SOCK_STREAM) {
      if (listen(sockfd, SOMAXCONN) < 0) {
        perror("listen");
        closesocket(sockfd);
        return NULL;
      }
    }
  }
  io = e_io_get(loop, sockfd);
  assert(io != NULL);
  io->io_type = type;
  if (side == EVENT_IO_SERVER_SIDE) {
    e_io_set_localaddr(io, &addr.sa, e_sockaddr_len(&addr));
    io->priority = EVENT_HIGH_PRIORITY;
  } else {
    e_io_set_peeraddr(io, &addr.sa, e_sockaddr_len(&addr));
  }
  return io;
}