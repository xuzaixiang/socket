#ifndef EVENT_H
#define EVENT_H

#ifdef EVENT_OS_WIN
#include "win/event_win.h"
#else
#include "posix/event_posix.h"
#ifdef EVENT_OS_LINUX
#include "linux/linux.h"
#else
#include "mac/mac.h"
#endif
#endif

#ifndef e_getpid
#error "undefined e_getpid"
#endif
#ifndef e_gettid
#error "undefined e_gettid"
#endif

#endif