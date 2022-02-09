//
// Created by 20123460 on 2022/2/9.
//

#include "../e_event.h"
#include <sys/epoll.h>

EVENT_ARRAY_DECL(struct epoll_event, events)

#define EVENTS_INIT_SIZE 64

typedef struct e_epoll_ctx_s {
  int epfd;
  struct events events;
} e_epoll_ctx_t;

int e_iowatcher_init(e_loop_t *loop) {
  if (loop->iowatcher)
    return 0;
  e_epoll_ctx_t *epoll_ctx;
  EVENT_ALLOC_SIZEOF(epoll_ctx);
  if ((epoll_ctx->epfd = epoll_create(EVENTS_INIT_SIZE)) < 0) {
    loop->iowatcher = NULL;
    EVENT_FREE(epoll_ctx);
    return -1;
  }
  events_init(&epoll_ctx->events, EVENTS_INIT_SIZE);
  loop->iowatcher = epoll_ctx;
  return 0;
}

int e_iowatcher_cleanup(e_loop_t *loop) {
  if (loop->iowatcher == NULL)
    return 0;
  e_epoll_ctx_t *epoll_ctx = (e_epoll_ctx_t *)loop->iowatcher;
  if (close(epoll_ctx->epfd) < 0) {
    return -1;
  }
  events_cleanup(&epoll_ctx->events);
  EVENT_FREE(loop->iowatcher);
  return 0;
}