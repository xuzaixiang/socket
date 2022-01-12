#ifndef EVENT_POSIX_H
#define EVENT_POSIX_H

#include <unistd.h>
#include "../e_config.h"
#include "e_pthread.h"

// getpid
#define e_getpid (long)getpid

// gettid
#if HAVE_GETTID // in unistd.h
#define e_gettid (long)gettid
#else
#define e_gettid   (long)pthread_self // in pthread.h
#endif
//#include <sys/syscall.h>
//#define e_gettid() (long)syscall(SYS_gettid)

// closesocket
#define closesocket(fd) close(fd) // in unistd.h


#define e_sleep(s)     sleep(s)
#define e_msleep(ms)   usleep((ms) * 1000)
#define e_usleep(us)   usleep(us)
#define e_delay(ms)    e_msleep(ms)

#endif

