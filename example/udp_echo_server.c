//
// Created by 20123460 on 2022/1/25.
//
#include <event/e_loop.h>
#include <event/e_io.h>
#include "event/e_sockaddr.h"

static void on_recvfrom(e_io_t* io, void* buf, int readbytes) {
  printf("on_recvfrom fd=%d readbytes=%d\n", e_io_fd(io), readbytes);
  char localaddrstr[EVENT_SOCKADDR_STRLEN] = {0};
  char peeraddrstr[EVENT_SOCKADDR_STRLEN] = {0};
  printf("[%s] <=> [%s]\n",
         EVENT_SOCKADDR_STR(e_io_localaddr(io), localaddrstr),
         EVENT_SOCKADDR_STR(e_io_peeraddr(io), peeraddrstr));

  char* str = (char*)buf;
  printf("< %.*s", readbytes, str);
  // echo
  printf("> %.*s", readbytes, str);
//  e_io_write(io, buf, readbytes);
}

int main() {
  const char* host = "0.0.0.0";
  int port = 8080;
  e_loop_t* loop = e_loop_new(0);
  e_io_t* io = e_loop_create_udp_server(loop, host, port);
  if (io == NULL) {
    return -20;
  }
  e_io_setcb_read(io, on_recvfrom);
  e_io_read(io);
  e_loop_run(loop);
  e_loop_free(&loop);
  return 0;
}