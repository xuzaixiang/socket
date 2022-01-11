

#ifndef EVENT_SOCKET_H
#define EVENT_SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>

typedef union {
  struct sockaddr sa;
  struct sockaddr_in sin;
  struct sockaddr_in6 sin6;
//#ifdef ENABLE_UDS
//  struct sockaddr_un  sun;
//#endif
} sockaddr_u;

int resolve_addr(const char* host, sockaddr_u* addr);

int sockaddr_set_ip(sockaddr_u *addr, const char *host);
void sockaddr_set_port(sockaddr_u *addr, int port);
int sockaddr_set_ipport(sockaddr_u *addr, const char *host, int port);

#endif //EVENT_SOCKET_H