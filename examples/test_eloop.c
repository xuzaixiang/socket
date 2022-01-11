
#define EVENT_ALLOC_COUNT
#define EVENT_ALLOC_PRINT

#include <event/e_alloc.h>
#include <event/e_atomic.h>
#include <event/e_loop.h>

int main() {
#ifdef EVENT_ALLOC_COUNT
  E_MEMCHECK;
#endif
  eloop_t *loop = e_loop_new(0);
  // test idle and priority
  for (int i = E_LOWEST_PRIORITY; i <= E_HIGHEST_PRIORITY; ++i) {
//    hidle_t *idle = hidle_add(loop, on_idle, 10);
//    hevent_set_priority(idle, i);
  }

  return 0;
}