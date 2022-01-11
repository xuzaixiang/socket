#include "iowatcher.h"

//#include "cus/cus_alloc.h"
#include "cus/algorithm/cus_array.h"

#if defined(CUS_OS_LINUX)
#define EVENT_EPOLL
#endif

#ifdef EVENT_EPOLL
#include "eevent.h"
#include <sys/epoll.h>

#define EVENTS_INIT_SIZE    64
//
//CUS_ARRAY_DECL(struct epoll_event, events);
//
//typedef struct epoll_ctx_s {
//  int epfd;
//  struct events       events;
//} epoll_ctx_t;
//
//int iowatcher_init(eloop_t *loop) {
//  if (loop->iowatcher) return 0;
//  epoll_ctx_t *epoll_ctx;
//  CUS_ALLOC_SIZEOF(epoll_ctx);
//  epoll_ctx->epfd = epoll_create(EVENTS_INIT_SIZE);
//  events_init(&epoll_ctx->events, EVENTS_INIT_SIZE);
//  loop->iowatcher = epoll_ctx;
//  return 0;
//}
//int iowatcher_cleanup(eloop_t* loop) {
//  if (loop->iowatcher == NULL) return 0;
//  epoll_ctx_t* epoll_ctx = (epoll_ctx_t*)loop->iowatcher;
//  close(epoll_ctx->epfd);
//  events_cleanup(&epoll_ctx->events);
//  CUS_FREE(loop->iowatcher);
//  return 0;
//}
//
//int iowatcher_add_event(eloop_t* loop, int fd, int events) {
//  if (loop->iowatcher == NULL) {
//    iowatcher_init(loop);
//  }
//  epoll_ctx_t* epoll_ctx = (epoll_ctx_t*)loop->iowatcher;
////  hio_t* io = loop->ios.ptr[fd];
////
////  struct epoll_event ee;
////  memset(&ee, 0, sizeof(ee));
////  ee.data.fd = fd;
////  // pre events
////  if (io->events & HV_READ) {
////    ee.events |= EPOLLIN;
////  }
////  if (io->events & HV_WRITE) {
////    ee.events |= EPOLLOUT;
////  }
////  // now events
////  if (events & HV_READ) {
////    ee.events |= EPOLLIN;
////  }
////  if (events & HV_WRITE) {
////    ee.events |= EPOLLOUT;
////  }
////  int op = io->events == 0 ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
////  epoll_ctl(epoll_ctx->epfd, op, fd, &ee);
////  if (op == EPOLL_CTL_ADD) {
////    if (epoll_ctx->events.size == epoll_ctx->events.maxsize) {
////      events_double_resize(&epoll_ctx->events);
////    }
////    epoll_ctx->events.size++;
////  }
//  return 0;
//}

#endif