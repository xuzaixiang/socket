//
// Created by 20123460 on 2022/2/8.
//

#ifndef EVENT_H
#define EVENT_H

#include "e_array.h"
#include "e_config.h"
#include "e_iowatcher.h"
#include "e_math.h"
#include "e_mutex.h"
#include "e_platform.h"
#include "e_queue.h"
#include "loop/e_custom_fd.h"
#include "stc_vector.h"
#include <event/e_loop.h>

#ifndef NDEBUG
#include <stdio.h>
#endif

#ifdef EVENT_OS_UNIX
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#ifdef EVENT_OS_LINUX
#include <sys/syscall.h>
#if EVENT_HAVE_EVENTFD
#include <sys/eventfd.h>
#endif // EVENT_HAVE_EVENTFD
#endif // EVENT_OS_LINUX
#endif // EVENT_OS_UNIX

#ifndef EVENT_CLOSE
#define EVENT_CLOSE(fd)                                                        \
  do {                                                                         \
    if ((fd) >= 0) {                                                           \
      close(fd);                                                               \
      (fd) = -1;                                                               \
    }                                                                          \
  } while (0)
#endif

#define EVENT_IO_ARRAY_INIT_SIZE 1024
// EVENT_ARRAY_DECL(e_io_t *, io_array)
STC_VECTOR(e_io_t *, io)

#define EVENT_CUSTOM_EVENT_QUEUE_INIT_SIZE 16
EVENT_QUEUE_DECL(e_event_t, event_queue)

struct e_loop_s {
  uint32_t flags;
  e_loop_status_t status;
  long pid;
  long tid;

  uint32_t nactives; // num of active io
  uint32_t npendings;
  // pendings: with priority as array.index
  e_event_t *pendings[EVENT_PRIORITY_SIZE];
  stc_vector_t *ios; // ios: with fd as array.index
  uint32_t nios;       // num of io

  void *iowatcher;

  // custom_events
#if defined(EVENT_OS_UNIX) && EVENT_HAVE_EVENTFD
  int custom_fd;
#else
  int custom_fd[2];
#endif
  event_queue custom_events;
  e_mutex_t custom_events_mutex;

  // loop
  e_cond_t loop_cond;
  e_mutex_t loop_mutex;
};

struct e_io_s {
  EVENT_FIELDS
  unsigned ready : 1;
  unsigned closed : 1;
  int fd;
  int events;
  int revents;
  e_read_cb read_cb;
  e_write_cb write_cb;
};

void e_io_ready(e_io_t *io);
void e_io_init(e_io_t *io);
void e_io_handle(e_io_t *io);

void e_loop_check_io_size(e_loop_t *loop, int fd);
void e_loop_handle(e_loop_t *loop);

#define EVENT_ACTIVE(ev)                                                       \
  if (!(ev)->active) {                                                         \
    (ev)->active = 1;                                                          \
    (ev)->loop->nactives++;                                                    \
  }

#define EVENT_INACTIVE(ev)                                                     \
  if ((ev)->active) {                                                          \
    (ev)->active = 0;                                                          \
    (ev)->loop->nactives--;                                                    \
  }

#define EVENT_ADD(loop, ev, cb)                                                \
  do {                                                                         \
    (ev)->loop = loop;                                                         \
    (ev)->cb = (e_event_cb)(cb);                                               \
    EVENT_ACTIVE(ev);                                                          \
  } while (0)

#define EVENT_PENDING(ev)                                                      \
  do {                                                                         \
    if (!(ev)->pending) {                                                      \
      (ev)->pending = 1;                                                       \
      (ev)->loop->npendings++;                                                 \
      e_event_t **phead =                                                      \
          &(ev)->loop->pendings[EVENT_PRIORITY_INDEX((ev)->priority)];         \
      (ev)->pending_next = *phead;                                             \
      *phead = (e_event_t *)(ev);                                              \
    }                                                                          \
  } while (0)

#define EVENT_DEL(ev)                                                          \
  do {                                                                         \
    EVENT_INACTIVE(ev);                                                        \
    if (!(ev)->pending) {                                                      \
      EVENT_FREE(ev);                                                          \
    }                                                                          \
  } while (0)

#endif // EVENT_H
