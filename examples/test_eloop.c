
#define EVENT_ALLOC_COUNT
#define EVENT_ALLOC_PRINT

#include <event/ealloc.h>
#include <event/eatomic.h>
#include <event/eloop.h>

int main() {
#ifdef EVENT_ALLOC_COUNT
    E_MEMCHECK;
    printf("--------");
#endif

    eloop_t *loop = eloop_new(0);



    return 0;
}