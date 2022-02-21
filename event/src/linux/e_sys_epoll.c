//
// Created by 20123460 on 2022/2/9.
//

#include "../e_event.h"
#include <sys/epoll.h>

STC_VECTOR(struct epoll_event, events)

#define EVENTS_INIT_SIZE 64

typedef struct e_epoll_ctx_s {
  int epfd;
  stc_vector_t *events;
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
  epoll_ctx->events = stc_vector_new_of_events(EVENTS_INIT_SIZE);
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
  stc_vector_free(&epoll_ctx->events);
  // epoll_ctx
  EVENT_FREE(loop->iowatcher);
  return 0;
}
int e_iowatcher_add_event(e_loop_t *loop, int fd, int events) {
  if (loop->iowatcher == NULL && (e_iowatcher_init(loop) < 0))
    return -1;
  e_epoll_ctx_t *epoll_ctx = (e_epoll_ctx_t *)loop->iowatcher;
  e_io_t *io = stc_vector_get_of_io(loop->ios, fd);
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
    stc_vector_resize(epoll_ctx->events,
                      stc_vector_size(epoll_ctx->events) + 1);
  }
  return 0;
}

int e_iowatcher_del_event(e_loop_t *loop, int fd, int events) {
  e_epoll_ctx_t *epoll_ctx = (e_epoll_ctx_t *)loop->iowatcher;
  if (epoll_ctx == NULL)
    return -1;
  e_io_t *io = stc_vector_get(loop->ios, fd);
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
    stc_vector_pop_back(epoll_ctx->events);
  }
  return 0;
}

int e_iowatcher_poll_events(e_loop_t *loop, int timeout) {
  e_epoll_ctx_t *epoll_ctx = (e_epoll_ctx_t *)loop->iowatcher;
  if (epoll_ctx == NULL)
    return -1;
  if (stc_vector_empty(epoll_ctx->events))
    return 0;
  int epoll_timeout = 0;
  if (timeout == EVENT_IOWATCHER_BLOCK_TIME_INDEFINITELY) {
    epoll_timeout = -1;
  } else {
    epoll_timeout = timeout;
  }
  printf("-------- 1111111 \n ");
  int nepoll =
      epoll_wait(epoll_ctx->epfd, stc_vector_data_of_events(epoll_ctx->events),
                 (int)stc_vector_size(epoll_ctx->events), epoll_timeout);
  printf("-------- \n ");
  if (nepoll < 0) {
    if (errno == EINTR) {
      return 0;
    }
    return nepoll;
  }
  if (nepoll == 0)
    return 0;
  int nevents = 0;
  for (int i = 0; i < stc_vector_size(epoll_ctx->events); ++i) {
    struct epoll_event ee = stc_vector_get_of_events(epoll_ctx->events, i);
    int fd = ee.data.fd;
    if (ee.events) {
      ++nevents;
      e_io_t *io = stc_vector_get_of_io(loop->ios, fd);
      if (io) {
        if (ee.events & (EPOLLIN | EPOLLHUP | EPOLLERR)) {
          io->revents |= EVENT_READ;
        }
        if (ee.events & (EPOLLOUT | EPOLLHUP | EPOLLERR)) {
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