//
// Created by 20123460 on 2022/2/8.
//

#ifndef EVENT_H
#define EVENT_H

#include "e_array.h"
#include "e_config.h"
#include "e_io.h"
#include "e_iowatcher.h"
#include "e_mutex.h"
#include "e_platform.h"
#include "e_queue.h"
#include <event/e_loop.h>

#ifdef EVENT_OS_UNIX
#include <pthread.h>
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
EVENT_ARRAY_DECL(e_io_t *, io_array)

#define EVENT_CUSTOM_EVENT_QUEUE_INIT_SIZE 16
EVENT_QUEUE_DECL(e_event_t, event_queue)

struct e_loop_s {
  uint32_t flags;
  e_loop_status_t status;
  long pid;
  long tid;

  // ios: with fd as array.index
  struct io_array ios;
  uint32_t nios;
  void *iowatcher;

  // custom_events
#if defined(EVENT_OS_UNIX) && EVENT_HAVE_EVENTFD
  int custom_fd;
#else
  int custom_fd[2];
#endif
  event_queue custom_events;
  e_mutex_t custom_events_mutex;
};

#include "e_custom_fd.h"

#endif // EVENT_H
