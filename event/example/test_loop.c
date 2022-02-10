//
// Created by 20123460 on 2022/2/9.
//
#include <event/e_loop.h>

int main() {
  e_loop_t *loop = e_loop_new(0);
  e_loop_run(loop);
  e_loop_free(&loop);
  return 0;
}