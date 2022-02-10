//
// Created by 20123460 on 2022/2/10.
//

#include "../e_event.h"

void e_loop_reset_custom_fd(e_loop_t *loop) {
#if defined(EVENT_OS_WIN)
#elif defined(EVENT_OS_UNIX) && EVENT_HAVE_EVENTFD
  loop->custom_fd = -1;
#else
  loop->custom_fd[0] = loop->custom_fd[1] = -1;
#endif
}

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

void e_loop_handle_custom_event(e_io_t* io){

}