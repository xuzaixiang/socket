#ifndef EVENT_H
#define EVENT_H

#include "e_def.h"
#include "e_platform.h"

int socket_errno();


#ifdef EVENT_OS_WIN
#include "win/e_win.h"
#else
#include <stddef.h>
#include "posix/e_posix.h"
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