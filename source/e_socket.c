//
// Created by 20123460 on 2022/1/15.
//

#include "e_socket.h"
#include "event/e_sockaddr.h"
#include <errno.h>

e_io_type_t e_socket_type(int fd) {
  int type = 0;
  socklen_t optlen = sizeof(int);
  int ret = getsockopt(fd, SOL_SOCKET, SO_TYPE, (char *) &type, &optlen);
//  printd("getsockopt SO_TYPE fd=%d ret=%d type=%d errno=%d\n", fd, ret, type, socket_errno());
  if (ret == 0) {
    switch (type) {
      case SOCK_STREAM:return EVENT_IO_TYPE_TCP;
      case SOCK_DGRAM:return EVENT_IO_TYPE_UDP;
      case SOCK_RAW:return EVENT_IO_TYPE_IP;
      default:return EVENT_IO_TYPE_SOCKET;
    }
  } else if (e_socket_errno() == ENOTSOCK) {
    switch (fd) {
      case 0: return EVENT_IO_TYPE_STDIN;
      case 1: return EVENT_IO_TYPE_STDOUT;
      case 2: return EVENT_IO_TYPE_STDERR;
      default: return EVENT_IO_TYPE_FILE;
    }
  } else {
    return EVENT_IO_TYPE_TCP;
  }
}

int e_socket_errno() {
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

void e_socket_init(e_io_t *io) {
  io->io_type = e_socket_type(io->fd);
  if ((io->io_type & EVENT_IO_TYPE_SOCK_DGRAM) || (io->io_type & EVENT_IO_TYPE_SOCK_RAW)) {
    // NOTE: sendto multiple peeraddr cannot use io->write_queue
    blocking(io->fd);
  } else {
    nonblocking(io->fd);
  }
  // fill io->localaddr io->peeraddr
  if (io->localaddr == NULL) {
    EVENT_ALLOC(io->localaddr, sizeof(e_sockaddr_t));
  }
  if (io->peeraddr == NULL) {
    EVENT_ALLOC(io->peeraddr, sizeof(e_sockaddr_t));
  }
  socklen_t addrlen = sizeof(e_sockaddr_t);
  int ret = getsockname(io->fd, io->localaddr, &addrlen);
//  printd("getsockname fd=%d ret=%d errno=%d\n", io->fd, ret, e_socket_errno());
  // NOTE: udp peeraddr set by recvfrom/sendto
  if (io->io_type & EVENT_IO_TYPE_SOCK_STREAM) {
    addrlen = sizeof(e_sockaddr_t);
    ret = getpeername(io->fd, io->peeraddr, &addrlen);
//    printd("getpeername fd=%d ret=%d errno=%d\n", io->fd, ret, e_socket_errno());
  }
}