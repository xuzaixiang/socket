//
// Created by 20123460 on 2022/1/26.
//
#include <event/e_io.h>
#include <event/e_loop.h>

int main() {
  const char *host = "0.0.0.0";
  int port = 8080;
  e_loop_t *loop = e_loop_new(0);
  e_io_t *io = e_loop_create_udp_client(loop, host, port);
  if (io == NULL) {
    return -20;
  }
  const char *test = "test messages";
//  sendto(io->fd, test, strlen(test), 0, io->peeraddr, EVENT_SOCKADDR_LEN(io->peeraddr));
  e_io_write(io,test, strlen(test));
//  recvfrom(io->fd,)

//  e_io_setcb_read(io, on_recvfrom);
//  e_io_read(io);
//  e_loop_run(loop);
//  e_loop_free(&loop);
  return 0;
}