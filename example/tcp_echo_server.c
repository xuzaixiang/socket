//
// Created by 20123460 on 2022/1/13.
//
#include "event/e_loop.h"
#include "stdatomic.h"

int main(int argc, char **argv) {
  e_loop_t* loop = e_loop_new(0);
  atomic_long a = ATOMIC_VAR_INIT(0L);
  _Atomic long counter = ATOMIC_VAR_INIT(0L);
  return 0;
}