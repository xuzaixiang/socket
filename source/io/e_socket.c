

#include <arpa/inet.h>
#include <netdb.h>
#include <stddef.h>
#include <string.h>
#include <event/e_alloc.h>
#include "event/io/e_socket.h"

int resolve_addr(const char* host, sockaddr_u* addr){
#ifdef OS_WIN
  WSAInit();
#endif
  if (inet_pton(AF_INET, host, &addr->sin.sin_addr) == 1) {
    addr->sa.sa_family = AF_INET; // host is ipv4, so easy ;)
    return 0;
  }

#ifdef ENABLE_IPV6
  if (inet_pton(AF_INET6, host, &addr->sin6.sin6_addr) == 1) {
        addr->sa.sa_family = AF_INET6; // host is ipv6
    }
    struct addrinfo* ais = NULL;
    struct addrinfo hint;
    hint.ai_flags = 0;
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = 0;
    hint.ai_protocol = 0;
    int ret = getaddrinfo(host, NULL, NULL, &ais);
    if (ret != 0 || ais == NULL || ais->ai_addrlen == 0 || ais->ai_addr == NULL) {
        printd("unknown host: %s err:%d:%s\n", host, ret, gai_strerror(ret));
        return ret;
    }
    memcpy(addr, ais->ai_addr, ais->ai_addrlen);
    freeaddrinfo(ais);
#else
  struct hostent* phe = gethostbyname(host);
  if (phe == NULL) {
    printd("unknown host %s err:%d\n", host, h_errno);
    return -h_errno;
  }
  addr->sin.sin_family = AF_INET;
  memcpy(&addr->sin.sin_addr, phe->h_addr_list[0], phe->h_length);
#endif
  return 0;
}

int sockaddr_set_ip(sockaddr_u *addr, const char *host) {
  if (!host || *host == '\0') {
    addr->sin.sin_family = AF_INET;
    addr->sin.sin_addr.s_addr = htonl(INADDR_ANY);
    return 0;
  }
  return resolve_addr(host, addr);
}

void sockaddr_set_port(sockaddr_u *addr, int port) {
  if (addr->sa.sa_family == AF_INET) {
    addr->sin.sin_port = htons(port);
  } else if (addr->sa.sa_family == AF_INET6) {
    addr->sin6.sin6_port = htons(port);
  }
}

int sockaddr_set_ipport(sockaddr_u *addr, const char *host, int port) {
//#ifdef ENABLE_UDS
//  if (port < 0) {
//        sockaddr_set_path(addr, host);
//        return 0;
//    }
//#endif
  int ret = sockaddr_set_ip(addr, host);
  if (ret != 0) return ret;
  sockaddr_set_port(addr, port);
  // SOCKADDR_PRINT(addr);
  return 0;
}