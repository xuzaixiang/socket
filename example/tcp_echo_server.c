//
// Created by 20123460 on 2022/1/13.
//
#include "event/e_loop.h"
#include "stdatomic.h"

void accept_tcp(e_io_t *io) {
  printfd("accept_tcp ------------------------- \n");
}

int main(int argc, char **argv) {
  e_loop_t *loop = e_loop_new(0);
  e_loop_create_tcp_server(loop, "0.0.0.0", 8080, accept_tcp);
  e_loop_run(loop);
  return 0;
}