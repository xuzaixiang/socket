//
// Created by 20123460 on 2022/2/9.
//

#include "../e_event.h"
#include <sys/epoll.h>

STC_VECTOR(struct epoll_event, events)

#define EVENTS_INIT_SIZE 64

typedef struct e_epoll_ctx_s {
  int epfd;
  struct events events;
} e_epoll_ctx_t;

int e_iowatcher_init(e_loop_t *loop) {
  if (loop->iowatcher)
    return 0;
  e_epoll_ctx_t *epoll_ctx;
  // epoll_ctx
  EVENT_ALLOC_SIZEOF(epoll_ctx);
  // epfd
  if ((epoll_ctx->epfd = epoll_create(EVENTS_INIT_SIZE)) < 0) {
    loop->iowatcher = NULL;
    EVENT_FREE(epoll_ctx);
    return -1;
  }
  // events
  events_new(&epoll_ctx->events);
  loop->iowatcher = epoll_ctx;
  return 0;
}

int e_iowatcher_cleanup(e_loop_t *loop) {
  if (loop->iowatcher == NULL)
    return 0;
  e_epoll_ctx_t *epoll_ctx = (e_epoll_ctx_t *)loop->iowatcher;
  // epfd
  if (close(epoll_ctx->epfd) < 0) {
    return -1;
  }
  // events
  events_free(&epoll_ctx->events);
  // epoll_ctx
  EVENT_FREE(loop->iowatcher);
  return 0;
}
int e_iowatcher_add_event(e_loop_t *loop, int fd, int events) {
  if (loop->iowatcher == NULL && (e_iowatcher_init(loop) < 0))
    return -1;
  e_epoll_ctx_t *epoll_ctx = (e_epoll_ctx_t *)loop->iowatcher;
  e_io_t *io = *io_array_get(&loop->ios, fd);
  // epoll event
  struct epoll_event ee;
  memset(&ee, 0, sizeof(ee));
  ee.data.fd = fd;
  if (io->events & EVENT_READ) {
    ee.events |= EPOLLIN;
  }
  if (io->events & EVENT_WRITE) {
    ee.events |= EPOLLOUT;
  }
  if (events & EVENT_READ) {
    ee.events |= EPOLLIN;
  }
  if (events & EVENT_WRITE) {
    ee.events |= EPOLLOUT;
  }
  int op = io->events == 0 ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
  epoll_ctl(epoll_ctx->epfd, op, fd, &ee);
  if (op == EPOLL_CTL_ADD) {
    events_push_back(&epoll_ctx->events, ee);
  }
  return 0;
}

int e_iowatcher_del_event(e_loop_t *loop, int fd, int events) {
  e_epoll_ctx_t *epoll_ctx = (e_epoll_ctx_t *)loop->iowatcher;
  if (epoll_ctx == NULL)
    return -1;
  e_io_t *io = *io_array_get(&loop->ios, fd);
  // epoll event
  struct epoll_event ee;
  memset(&ee, 0, sizeof(ee));
  ee.data.fd = fd;
  if (io->events & EVENT_READ) {
    ee.events |= EPOLLIN;
  }
  if (io->events & EVENT_WRITE) {
    ee.events |= EPOLLOUT;
  }
  if (events & EVENT_READ) {
    ee.events &= ~EPOLLIN;
  }
  if (events & EVENT_WRITE) {
    ee.events &= ~EPOLLOUT;
  }
  int op = ee.events == 0 ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;
  epoll_ctl(epoll_ctx->epfd, op, fd, &ee);
  if (op == EPOLL_CTL_DEL) {
    events_pop_back(&epoll_ctx->events);
  }
  return 0;
}

int e_iowatcher_poll_events(e_loop_t *loop, int timeout) {
  e_epoll_ctx_t *epoll_ctx = (e_epoll_ctx_t *)loop->iowatcher;
  if (epoll_ctx == NULL)
    return -1;
  if (events_empty(&epoll_ctx->events))
    return 0;
  int nepoll = epoll_wait(epoll_ctx->epfd, events_data(&epoll_ctx->events),
                          (int)events_size(&epoll_ctx->events), timeout);
  if (nepoll < 0) {
    if (errno == EINTR) {
      return 0;
    }
    return nepoll;
  }
  if (nepoll == 0)
    return 0;
  int nevents = 0;
  for (int i = 0; i < events_size(&epoll_ctx->events); ++i) {
    struct epoll_event *ee = events_get(&epoll_ctx->events, i);
    int fd = ee->data.fd;
    if (ee->events) {
      ++nevents;
      e_io_t *io = *io_array_get(&loop->ios, fd);
      if (io) {
        if (ee->events & (EPOLLIN | EPOLLHUP | EPOLLERR)) {
          io->revents |= EVENT_READ;
        }
        if (ee->events & (EPOLLOUT | EPOLLHUP | EPOLLERR)) {
          io->revents |= EVENT_WRITE;
        }
        EVENT_PENDING(io);
      }
    }
    if (nevents == nepoll)
      break;
  }
  return nevents;
}