//
// Created by 20123460 on 2022/2/9.
//

//#include "e_platform.h"

#include "e_event.h"

long e_get_pid() {
#ifdef EVENT_OS_WIN
  return (long)GetCurrentProcessId
#else
  return (long)getpid();
#endif
}

long e_get_tid() {
#ifdef EVENT_OS_WIN
  return (long)GetCurrentThreadId();
#elif EVENT_HAVE_GETTID || defined(EVENT_OS_ANDROID)
  return (long)gettid();
#elif defined(EVENT_OS_LINUX)
  return (long)syscall(SYS_gettid);
#else
  // pthread_t是POSIX定义的线程id，并不是实际系统内核的线程ID
  // 进程内唯一，不同进程不唯一
  return (long)pthread_self();
#endif
}