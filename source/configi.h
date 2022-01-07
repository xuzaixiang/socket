#ifndef EVENT_CONFIGI_H
#define EVENT_CONFIGI_H

#include "config.h"

#ifndef __cplusplus

#if HAVE_STDBOOL_H
#include <stdbool.h>
#else

#ifndef bool
#define bool char
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif
#endif //HAVE_STDBOOL_H

#endif //__cplusplus

#if HAVE_STDINT_H
#include <stdint.h>
#elif defined(_MSC_VER) && _MSC_VER < 1700
typedef __int8              int8_t;
typedef __int16             int16_t;
typedef __int32             int32_t;
typedef __int64             int64_t;
typedef unsigned __int8     uint8_t;
typedef unsigned __int16    uint16_t;
typedef unsigned __int32    uint32_t;
typedef unsigned __int64    uint64_t;
#endif

typedef float float32_t;
typedef double float64_t;
typedef int (*method_t)(void *userdata);
typedef void (*procedure_t)(void *userdata);

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#if HAVE_SYS_TIME_H
#include <sys/time.h>   // for gettimeofday
#endif

#if HAVE_FCNTL_H
#include <fcntl.h>
#endif

#if HAVE_PTHREAD_H
#include <pthread.h>
#endif

#endif //EVENT_CONFIGI_H