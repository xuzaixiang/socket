#ifndef EVENT_EVENT_H
#define EVENT_EVENT_H

#include <errno.h>
#include <stdint.h>//c99

#include "macro/e_platform.h"
#include "macro/e_def.h"
#include "macro/e_export.h"
#include "util/e_alloc.h"
#include "al/e_array.h"
#include "al/e_queue.h"

#include "e_iowatcher.h"

#define EVENT_READ  0x0001
#define EVENT_WRITE 0x0004
#define EVENT_RDWR  (EVENT_READ|EVENT_WRITE)

#define EVENT_READ_INDEX  0
#define EVENT_WRITE_INDEX 1

#ifndef EVENT_TIME_INFINITE
#define EVENT_TIME_INFINITE    -1
#endif

#define EVENT_LOWEST_PRIORITY    (-5)
#define EVENT_LOW_PRIORITY       (-3)
#define EVENT_NORMAL_PRIORITY      0
#define EVENT_HIGH_PRIORITY        3
#define EVENT_HIGHEST_PRIORITY     5
#define EVENT_PRIORITY_SIZE  (EVENT_HIGHEST_PRIORITY-EVENT_LOWEST_PRIORITY+1)
#define EVENT_PRIORITY_INDEX(priority) (priority-EVENT_LOWEST_PRIORITY)

#if defined(EVENT_OS_WIN)
#else
#include "posix/e_posix.h"
#if defined(EVENT_OS_LINUX)
#elif defined(EVENT_OS_MAC)

#endif

typedef struct e_event_s e_event_t;
typedef void (*e_event_cb)(e_event_t *ev);

typedef enum {
  EVENT_TYPE_NONE = 0,
  EVENT_TYPE_IO = 0x00000001,
  EVENT_TYPE_TIMEOUT = 0x00000010,
  EVENT_TYPE_PERIOD = 0x00000020,
  EVENT_TYPE_TIMER = EVENT_TYPE_TIMEOUT | EVENT_TYPE_PERIOD,
  EVENT_TYPE_IDLE = 0x00000100,
  EVENT_TYPE_CUSTOM = 0x00000400, // 1024
} e_event_type_e;

#define EVENT_FLAGS        \
    unsigned    destroy :1; \
    unsigned    active  :1; \
    unsigned    pending :1;

#define EVENT_FIELDS                   \
    e_loop_t*             loop;           \
    e_event_type_e        event_type;     \
    uint64_t              event_id;       \
    e_event_cb            cb;             \
    void*                 userdata;       \
    void*                 privdata;    \
    struct e_event_s*      pending_next;   \
    int                   priority;       \
    EVENT_FLAGS

struct e_event_s {
  EVENT_FIELDS
};

uint64_t e_event_next_id();

#define EVENT_ACTIVE(ev) \
    if (!ev->active) {\
        ev->active = 1;\
        ev->loop->nactives++;\
    }

#define EVENT_INACTIVE(ev) \
    if (ev->active) {\
        ev->active = 0;\
        ev->loop->nactives--;\
    }\

#define EVENT_ADD(loop, ev, cb) \
    do {\
        ev->loop = loop;\
        ev->event_id = e_event_next_id();\
        ev->cb = (e_event_cb)cb;\
        EVENT_ACTIVE(ev);\
    } while(0)

#endif

#define EVENT_DEL(ev) \
    do {\
        EVENT_INACTIVE(ev);\
        if (!ev->pending) {\
            EVENT_FREE(ev);\
        }\
    } while(0)

#define EVENT_PENDING(ev) \
    do {\
        if (!ev->pending) {\
            ev->pending = 1;\
            ev->loop->npendings++;\
            e_event_t** phead = &ev->loop->pendings[EVENT_PRIORITY_INDEX(ev->priority)];\
            ev->pending_next = *phead;\
            *phead = (e_event_t*)ev;\
        }\
    } while(0)

#endif //EVENT_EVENT_H