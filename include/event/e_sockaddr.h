//
// Created by 20123460 on 2022/1/15.
//

#ifndef EVENT__SOCKADDR_H
#define EVENT__SOCKADDR_H

#include "e_event.h"

//#define INET_ADDRSTRLEN   16
//#define INET6_ADDRSTRLEN  46
#ifdef EVENT_ENABLE_UDS
#define EVENT_SOCKADDR_STRLEN     sizeof(((struct sockaddr_un*)(NULL))->sun_path)
HV_INLINE void sockaddr_set_path(sockaddr_u* addr, const char* path) {
    addr->sa.sa_family = AF_UNIX;
    strncpy(addr->sun.sun_path, path, sizeof(addr->sun.sun_path));
}
#else
#define EVENT_SOCKADDR_STRLEN     64 // ipv4:port | [ipv6]:port
#endif



typedef union e_sockaddr_u {
  struct sockaddr sa;
  struct sockaddr_in sin;
  struct sockaddr_in6 sin6;
} e_sockaddr_t;

EVENT_EXPORT int e_sockaddr_resolve(const char *host, e_sockaddr_t *addr);
EVENT_EXPORT int e_sockaddr_set_ip(e_sockaddr_t *addr, const char *host);
EVENT_EXPORT void e_sockaddr_set_port(e_sockaddr_t *addr, int port);
EVENT_EXPORT int e_sockaddr_set_ipport(e_sockaddr_t *addr, const char *host, int port);
EVENT_EXPORT const char* e_sockaddr_str(e_sockaddr_t* addr, char* buf, int len);
EVENT_EXPORT socklen_t e_sockaddr_len(e_sockaddr_t* addr);
EVENT_EXPORT void e_sockaddr_print(e_sockaddr_t *addr);

#define EVENT_SOCKADDR_PRINT(addr)  e_sockaddr_print((e_sockaddr_t*)addr)
#define EVENT_SOCKADDR_STR(addr, buf) e_sockaddr_str((e_sockaddr_t*)addr, buf, sizeof(buf))
#define EVENT_SOCKADDR_LEN(addr)      e_sockaddr_len((e_sockaddr_t*)addr)

#endif //EVENT__SOCKADDR_H
