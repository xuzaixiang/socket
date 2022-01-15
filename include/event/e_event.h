#ifndef EVENT_EVENT_H
#define EVENT_EVENT_H

#include <errno.h>
#include <stdint.h>//c99

#include "macro/e_platform.h"
#include "macro/e_def.h"
#include "macro/e_export.h"
#include "util/e_alloc.h"
#include "util/e_atomic.h"
#include "al/e_array.h"

#include "e_iowatcher.h"

#define EVENT_READ  0x0001
#define EVENT_WRITE 0x0004
#define EVENT_RDWR  (EVENT_READ|EVENT_WRITE)

#if defined(EVENT_OS_WIN)
#else
#include "posix/e_posix.h"
#if defined(EVENT_OS_LINUX)
#include <sys/socket.h>
#else

#endif
#endif

#endif //EVENT_EVENT_H