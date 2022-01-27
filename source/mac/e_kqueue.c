//
// Created by 20123460 on 2022/1/19.
//
#include "event/e_event.h"
#include "event/e_loop.h"

#if defined(EVENT_OS_MAC)
#include <sys/event.h>

#define EVENT_INIT_SIZE     64
#define EVENT_INDEX(type) ((type == EVFILT_READ) ? EVENT_READ_INDEX : EVENT_WRITE_INDEX)

typedef struct kqueue_ctx_s {
  int kqfd;
  int capacity;
  int nchanges;
  struct kevent *changes;
  //int nevents; // nevents == nchanges
  struct kevent *events;
} kqueue_ctx_t;

int e_iowatcher_init(e_loop_t *loop) {
  if (loop->iowatcher)
    return 0;
  // kqueue_ctx_t
  kqueue_ctx_t *kqueue_ctx;
  EVENT_ALLOC_SIZEOF(kqueue_ctx);
  kqueue_ctx->kqfd = kqueue();
  kqueue_ctx->capacity = EVENT_INIT_SIZE;
  kqueue_ctx->nchanges = 0;
  int bytes = (int) (sizeof(struct kevent)) * kqueue_ctx->capacity;
  EVENT_ALLOC(kqueue_ctx->changes, bytes);
  EVENT_ALLOC(kqueue_ctx->events, bytes);
  loop->iowatcher = kqueue_ctx;
  return 0;
}

int e_iowatcher_cleanup(e_loop_t *loop) {
  if (loop->iowatcher == NULL)
    return 0;
  kqueue_ctx_t *kqueue_ctx = (kqueue_ctx_t *) loop->iowatcher;
  close(kqueue_ctx->kqfd);
  EVENT_FREE(kqueue_ctx->changes);
  EVENT_FREE(kqueue_ctx->events);
  EVENT_FREE(loop->iowatcher);
  return 0;
}

static void kqueue_ctx_resize(kqueue_ctx_t *kqueue_ctx, int size) {
  int bytes = (int) sizeof(struct kevent) * size;
  kqueue_ctx->changes = (struct kevent *) e_realloc(kqueue_ctx->changes, bytes);
  kqueue_ctx->events = (struct kevent *) e_realloc(kqueue_ctx->events, bytes);
  kqueue_ctx->capacity = size;
}

int e_iowatcher_add_event(e_loop_t *loop, int fd, int events) {
  events = (unsigned int) events & (unsigned int) EVENT_READ
           ? EVFILT_READ : EVFILT_WRITE;
  if (loop->iowatcher == NULL) {
    e_iowatcher_init(loop);
  }
  kqueue_ctx_t *kqueue_ctx = (kqueue_ctx_t *) loop->iowatcher;
  e_io_t *io = loop->ios.ptr[fd];
  int idx = io->event_index[EVENT_INDEX(events)];
  if (idx < 0) {// add at the last of the kqueue_ctx events
    io->event_index[EVENT_INDEX(events)] = idx = kqueue_ctx->nchanges;
    kqueue_ctx->nchanges++;
    if (idx == kqueue_ctx->capacity) {// expand the size
      kqueue_ctx_resize(kqueue_ctx, kqueue_ctx->capacity * 2);
    }
    memset(kqueue_ctx->changes + idx, 0, sizeof(struct kevent));
    kqueue_ctx->changes[idx].ident = fd;
  }
  // assignment
  assert(kqueue_ctx->changes[idx].ident == fd);
  kqueue_ctx->changes[idx].filter = events;
  kqueue_ctx->changes[idx].flags = (uint16_t) EV_ADD | (uint16_t) EV_ENABLE;
  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = 0;
  kevent(kqueue_ctx->kqfd, kqueue_ctx->changes, kqueue_ctx->nchanges, NULL, 0, &ts);
  return 0;
}

int e_iowatcher_del_event(e_loop_t *loop, int fd, int events) {
  events = (unsigned int) events & (unsigned int) EVENT_READ
           ? EVFILT_READ : EVFILT_WRITE;
  kqueue_ctx_t *kqueue_ctx = (kqueue_ctx_t *) loop->iowatcher;
  if (kqueue_ctx == NULL)
    return 0;
  e_io_t *io = loop->ios.ptr[fd];
  int idx = io->event_index[EVENT_INDEX(events)];
  if (idx < 0)
    return 0;
  assert(kqueue_ctx->changes[idx].ident == fd);
  kqueue_ctx->changes[idx].flags = EV_DELETE;
  io->event_index[EVENT_INDEX(events)] = -1;
  int lastidx = kqueue_ctx->nchanges - 1;
  if (idx < lastidx) { // if fd is not the last of the kqueue_ctx
    // swap idx and lastidx
    struct kevent tmp;
    tmp = kqueue_ctx->changes[idx];
    kqueue_ctx->changes[idx] = kqueue_ctx->changes[lastidx];
    kqueue_ctx->changes[lastidx] = tmp;
    e_io_t *last = loop->ios.ptr[kqueue_ctx->changes[idx].ident];
    if (last) {
      last->event_index[EVENT_INDEX(kqueue_ctx->changes[idx].filter)] = idx;
    }
  }
  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = 0;
  kevent(kqueue_ctx->kqfd, kqueue_ctx->changes, kqueue_ctx->nchanges, NULL, 0, &ts);
  kqueue_ctx->nchanges--;
  return 0;
}

int e_iowatcher_poll_events(e_loop_t *loop, int timeout) {
  kqueue_ctx_t *kqueue_ctx = (kqueue_ctx_t *) loop->iowatcher;
  if (kqueue_ctx == NULL)
    return 0;
  if (kqueue_ctx->nchanges == 0)
    return 0;
  struct timespec ts, *tp;
  if (timeout == EVENT_TIME_INFINITE) {
    tp = NULL;
  } else {
    ts.tv_sec = timeout / 1000;
    ts.tv_nsec = (timeout % 1000) * 1000000;
    tp = &ts;
  }
  int nkqueue =
      kevent(kqueue_ctx->kqfd, kqueue_ctx->changes, kqueue_ctx->nchanges, kqueue_ctx->events, kqueue_ctx->nchanges, tp);
  if (nkqueue < 0) {
    perror("kevent < 0 \n");
    return nkqueue;
  }
  if (nkqueue == 0)
    return 0;
  int nevents = 0;
  for (int i = 0; i < nkqueue; ++i) {
    if (kqueue_ctx->events[i].flags & EV_ERROR) {
      continue;
    }
    ++nevents;
    int fd = kqueue_ctx->events[i].ident;
    int revents = kqueue_ctx->events[i].filter;
    e_io_t *io = loop->ios.ptr[fd];
    if (io) {
      if (revents & EVFILT_READ) {
        io->revents |= EVENT_READ;
      }
      if (revents & EVFILT_WRITE) {
        io->revents |= EVENT_WRITE;
      }
      EVENT_PENDING(io);
    }
    if (nevents == nkqueue)
      break;
  }
  return nevents;
}

#endif