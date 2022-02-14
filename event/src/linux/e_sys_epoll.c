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
int e_iowatcher_add_event(e_loop_t *loop, int fd, int events) {
  if (loop->iowatcher == NULL) {
    e_iowatcher_init(loop);
  }
  e_epoll_ctx_t *epoll_ctx = (e_epoll_ctx_t *)loop->iowatcher;
//  e_io_t *io = loop->ios.ptr[fd];
  e_io_t *io = io_array_get(&loop->ios,fd);
  struct epoll_event ee;
  memset(&ee, 0, sizeof(ee));
  ee.data.fd = fd;
  // pre events
  if (io->events & EVENT_READ) {
    ee.events |= EPOLLIN;
  }
  if (io->events & EVENT_WRITE) {
    ee.events |= EPOLLOUT;
  }
  // now events
  if (events & EVENT_READ) {
    ee.events |= EPOLLIN;
  }
  if (events & EVENT_WRITE) {
    ee.events |= EPOLLOUT;
  }
  int op = io->events == 0 ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
  epoll_ctl(epoll_ctx->epfd, op, fd, &ee);
  if (op == EPOLL_CTL_ADD) {
    if (epoll_ctx->events.size == epoll_ctx->events.maxsize) {
      events_double_resize(&epoll_ctx->events);
    }
    epoll_ctx->events.size++;
  }
  return 0;
}
int e_iowatcher_poll_events(e_loop_t *loop, int timeout){
  e_epoll_ctx_t* epoll_ctx = (e_epoll_ctx_t*)loop->iowatcher;
  if (epoll_ctx == NULL)  return 0;
  if (epoll_ctx->events.size == 0) return 0;
  int nepoll = epoll_wait(epoll_ctx->epfd, epoll_ctx->events.ptr, epoll_ctx->events.size, timeout);
  if (nepoll < 0) {
    if (errno == EINTR) {
      return 0;
    }
    return nepoll;
  }
  if (nepoll == 0) return 0;
  int nevents = 0;
  for (int i = 0; i < epoll_ctx->events.size; ++i) {
    struct epoll_event* ee = epoll_ctx->events.ptr + i;
    int fd = ee->data.fd;
    uint32_t revents = ee->events;
    if (revents) {
      ++nevents;
//      e_io_t* io = loop->ios.ptr[fd];
      e_io_t* io = io_array_get(&loop->ios,fd);
      if (io) {
        if (revents & (EPOLLIN | EPOLLHUP | EPOLLERR)) {
          io->revents |= EVENT_READ;
        }
        if (revents & (EPOLLOUT | EPOLLHUP | EPOLLERR)) {
          io->revents |= EVENT_WRITE;
        }
        EVENT_PENDING(io);
      }
    }
    if (nevents == nepoll) break;
  }
  return nevents;
}