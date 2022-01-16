//
// Created by 20123460 on 2022/1/15.
//
#include "event/e_sockaddr.h"
#include "stdio.h"

const char *e_sockaddr_str(e_sockaddr_t *addr, char *buf, int len) {
  char ip[EVENT_SOCKADDR_STRLEN] = {0};
  uint16_t port = 0;
  if (addr->sa.sa_family == AF_INET) {
    inet_ntop(AF_INET, &addr->sin.sin_addr, ip, len);
    port = ntohs(addr->sin.sin_port);
    snprintf(buf, len, "%s:%d", ip, port);
  } else if (addr->sa.sa_family == AF_INET6) {
    inet_ntop(AF_INET6, &addr->sin6.sin6_addr, ip, len);
    port = ntohs(addr->sin6.sin6_port);
    snprintf(buf, len, "[%s]:%d", ip, port);
  }
#ifdef EVENT_ENABLE_UDS
  else if (addr->sa.sa_family == AF_UNIX) {
        snprintf(buf, len, "%s", addr->sun.sun_path);
    }
#endif
  return buf;
}
socklen_t e_sockaddr_len(e_sockaddr_t *addr) {
  if (addr->sa.sa_family == AF_INET) {
    return sizeof(struct sockaddr_in);
  } else if (addr->sa.sa_family == AF_INET6) {
    return sizeof(struct sockaddr_in6);
  }
#ifdef ENABLE_UDS
  else if (addr->sa.sa_family == AF_UNIX) {
        return sizeof(struct sockaddr_un);
    }
#endif
  return sizeof(e_sockaddr_t);
}
void e_sockaddr_print(e_sockaddr_t *addr) {
  char buf[EVENT_SOCKADDR_STRLEN] = {0};
  e_sockaddr_str(addr, buf, sizeof(buf));
  puts(buf);
}

int e_sockaddr_resolve(const char *host, e_sockaddr_t *addr) {
#ifdef EVENT_OS_WIN
  WSAInit();
#endif
  if (inet_pton(AF_INET, host, &addr->sin.sin_addr) == 1) {
    addr->sa.sa_family = AF_INET;
    return 0;
  }
#ifdef EVENT_ENABLE_IPV6
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
  struct hostent *phe = gethostbyname(host);
  if (phe == NULL) {
    printf("unknown host %s err:%d\n", host, h_errno);
    return -h_errno;
  }
  addr->sin.sin_family = AF_INET;
  memcpy(&addr->sin.sin_addr, phe->h_addr_list[0], phe->h_length);
#endif
  return 0;
}

int e_sockaddr_set_ip(e_sockaddr_t *addr, const char *host) {
  if (!host || *host == '\0') {
    addr->sin.sin_family = AF_INET;
    addr->sin.sin_addr.s_addr = htonl(INADDR_ANY);
    return 0;
  }
  return e_sockaddr_resolve(host, addr);
}

void e_sockaddr_set_port(e_sockaddr_t *addr, int port) {
  if (addr->sa.sa_family == AF_INET) {
    addr->sin.sin_port = htons(port);
  } else if (addr->sa.sa_family == AF_INET6) {
    addr->sin6.sin6_port = htons(port);
  }
}

int e_sockaddr_set_ipport(e_sockaddr_t *addr, const char *host, int port) {
  int ret = e_sockaddr_set_ip(addr, host);
  if (ret != 0) return ret;
  e_sockaddr_set_port(addr, port);
  EVENT_SOCKADDR_PRINT(addr);
  return 0;
}