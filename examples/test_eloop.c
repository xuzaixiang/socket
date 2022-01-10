
#define EVENT_ALLOC_COUNT
#define EVENT_ALLOC_PRINT

#include <event/ealloc.h>
#include <event/eatomic.h>

int main() {
#ifdef EVENT_ALLOC_COUNT
  E_MEMCHECK;
  printf("--------");
#endif
  return 0;
}