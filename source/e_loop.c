//
// Created by 20123460 on 2022/1/13.
//

#include "event/e_loop.h"

e_loop_t *e_loop_new(uint32_t flags) {
  e_loop_t *loop;
  loop->flags |= flags;
  return loop;
}

