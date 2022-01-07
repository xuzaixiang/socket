#include "iowatcher.h"

#ifdef EVENT_EPOLL
#include "eplatform.h"
#include "event.h"
#include <sys/epoll.h>

#define EVENTS_INIT_SIZE    64

typedef struct epoll_ctx_s {
  int epfd;
//    struct
} epoll_ctx_t;

int iowatcher_init(eloop_t *loop) {
  if (loop->iowatcher) return 0;
  epoll_ctx_t *epoll_ctx;
  return 0;
}

#endif