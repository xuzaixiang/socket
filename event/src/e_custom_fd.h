//
// Created by 20123460 on 2022/2/9.
//

#ifndef EVENT_CUSTOM_FD_H
#define EVENT_CUSTOM_FD_H

#if defined(EVENT_OS_WIN)

// NOTE: Avoid duplication closesocket in hio_cleanup
// SAFE_CLOSESOCKET(loop->eventfds[EVENTFDS_READ_INDEX]);
SAFE_CLOSESOCKET(loop->eventfds[EVENTFDS_WRITE_INDEX]);

#elif defined(EVENT_OS_UNIX) && EVENT_HAVE_EVENTFD
#define EVENT_RESET_LOOP_CUSTOM_FD(loop) ((loop)->custom_fd = -1)
#define EVENT_CLOSE_LOOP_CUSTOM_FD(loop) EVENT_CLOSE((loop)->custom_fd)
#else
#define EVENT_RESET_LOOP_CUSTOM_FD                                             \
  ((loop)->custom_fd[0] = (loop)->custom_fd[1] = -1)
#define EVENT_CLOSE_LOOP_CUSTOM_FD(loop)                                       \
  do {                                                                         \
    EVENT_CLOSE((loop)->custom_fd[0]);                                         \
    EVENT_CLOSE((loop)->custom_fd[1]);                                         \
  } while (0)
#endif

#endif // EVENT_CUSTOM_FD_H
