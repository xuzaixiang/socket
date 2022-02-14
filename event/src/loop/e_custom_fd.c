//
// Created by 20123460 on 2022/2/10.
//

#include "../e_event.h"

void e_loop_close_custom_fd(e_loop_t *loop) {
#if defined(EVENT_OS_WIN)
  // NOTE: Avoid duplication closesocket in hio_cleanup
  // SAFE_CLOSESOCKET(loop->eventfds[EVENTFDS_READ_INDEX]);
  SAFE_CLOSESOCKET(loop->eventfds[EVENTFDS_WRITE_INDEX]);
#elif defined(EVENT_OS_UNIX) && EVENT_HAVE_EVENTFD
  EVENT_CLOSE(loop->custom_fd);
#else
  EVENT_CLOSE(loop->custom_fd[0]);
  EVENT_CLOSE(loop->custom_fd[1]);
#endif
}

int e_loop_create_custom_fd(e_loop_t *loop) {
#if defined(EVENT_OS_WIN)
  if (Socketpair(AF_INET, SOCK_STREAM, 0, loop->eventfds) != 0) {
    return -1;
  }
#elif defined(EVENT_OS_UNIX) && EVENT_HAVE_EVENTFD
  int efd = eventfd(0, 0);
  if (efd < 0) {
    return -1;
  }
  loop->custom_fd = efd;
#else
  if (pipe(loop->custom_fd) != 0) {
    return -1;
  }
#endif
  return 0;
}

int e_loop_get_custom_write_fd(e_loop_t *loop) {
#if defined(EVENT_OS_UNIX) && EVENT_HAVE_EVENTFD
  return loop->custom_fd;
#else
  return loop->custom_fd[1];
#endif
}
int e_loop_get_custom_read_fd(e_loop_t *loop) {
#if defined(EVENT_OS_UNIX) && EVENT_HAVE_EVENTFD
  return loop->custom_fd;
#else
  return loop->custom_fd[0];
#endif
}

void e_loop_handle_custom_event(e_io_t *io) {}

int e_loop_post(e_loop_t *loop, e_event_t *ev) {
  if (ev->loop == NULL) {
    ev->loop = loop;
  }
  if (ev->event_type == 0) {
    ev->event_type = EVENT_TYPE_CUSTOM;
  }
  e_mutex_lock(&loop->custom_events_mutex);
#if defined(EVENT_OS_UNIX) && EVENT_HAVE_EVENTFD
  uint64_t count = 1;
  size_t nwrite =
      write(e_loop_get_custom_write_fd(loop), &count, sizeof(count));
#elif defined(EVENT_OS_UNIX)
  nwrite = write(e_loop_get_custom_write_fd(loop), "e", 1);
#else
  nwrite = send(e_loop_get_custom_write_fd(loop), "e", 1, 0);
#endif
  if (nwrite <= 0)
    goto fail;
  event_queue_push_back(&loop->custom_events, ev);
  e_mutex_unlock(&loop->custom_events_mutex);
  return 0;
fail:
  e_mutex_unlock(&loop->custom_events_mutex);
  return -1;
}