#ifndef EVENT_POSIX_H
#define EVENT_POSIX_H

#include <unistd.h>
#include <pthread.h>
#include "../econfig.h"

#define e_getpid (long)getpid
#if HAVE_GETTID // in unistd.h
#define e_gettid (long)gettid
#else
#define e_gettid   (long)pthread_self
#endif
//#include <sys/syscall.h>
//#define e_gettid() (long)syscall(SYS_gettid)

#endif

