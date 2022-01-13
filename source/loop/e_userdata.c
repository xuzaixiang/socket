#include "event/e_loop.h"

void e_loop_set_userdata(e_loop_t *loop, void *userdata) {
  loop->userdata = userdata;
}
void *e_loop_userdata(e_loop_t *loop) {
  return loop->userdata;
}