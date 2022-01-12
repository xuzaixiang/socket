

#ifndef EVENT_SOCKET_H
#define EVENT_SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include "e_api.h"


//#define INET_ADDRSTRLEN   16
//#define INET6_ADDRSTRLEN  46
#ifdef ENABLE_UDS
#define SOCKADDR_STRLEN     sizeof(((struct sockaddr_un*)(NULL))->sun_path)
HV_INLINE void sockaddr_set_path(sockaddr_u* addr, const char* path) {
    addr->sa.sa_family = AF_UNIX;
    strncpy(addr->sun.sun_path, path, sizeof(addr->sun.sun_path));
}
#else
#define SOCKADDR_STRLEN     64 // ipv4:port | [ipv6]:port
#endif

#ifndef SAFE_CLOSESOCKET
#define SAFE_CLOSESOCKET(fd)  do {if ((fd) >= 0) {closesocket(fd); (fd) = -1;}} while(0)
#endif

typedef union {
  struct sockaddr sa;
  struct sockaddr_in sin;
  struct sockaddr_in6 sin6;
//#ifdef ENABLE_UDS
//  struct sockaddr_un  sun;
//#endif
} sockaddr_u;

int resolve_addr(const char* host, sockaddr_u* addr);

socklen_t sockaddr_len(sockaddr_u* addr);
int sockaddr_set_ip(sockaddr_u *addr, const char *host);
void sockaddr_set_port(sockaddr_u *addr, int port);
int sockaddr_set_ipport(sockaddr_u *addr, const char *host, int port);
const char* sockaddr_str(sockaddr_u* addr, char* buf, int len);

#define SOCKADDR_LEN(addr)      sockaddr_len((sockaddr_u*)addr)
#define SOCKADDR_STR(addr, buf) sockaddr_str((sockaddr_u*)addr, buf, sizeof(buf))
#define SOCKADDR_PRINT(addr)    sockaddr_print((sockaddr_u*)addr)

#endif //EVENT_SOCKET_H