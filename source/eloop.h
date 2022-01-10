#ifndef EVENT_ELOOP_H
#define EVENT_ELOOP_H

#include "stdint.h"//C99

#define EEVENT_LOWEST_PRIORITY    (-5)
#define EEVENT_HIGHEST_PRIORITY     5
#define EEVENT_PRIORITY_SIZE  (EEVENT_HIGHEST_PRIORITY-EEVENT_LOWEST_PRIORITY+1)
#define EEVENT_LOW_PRIORITY       (-3)
#define EEVENT_NORMAL_PRIORITY      0
#define EEVENT_HIGH_PRIORITY        3

typedef struct eloop_s eloop_t;
typedef struct eevent_s eevent_t;

typedef void (*eevent_cb)(eevent_t *ev);

typedef enum {
  ELOOP_STATUS_STOP,
  ELOOP_STATUS_RUNNING,
  ELOOP_STATUS_PAUSE
} eloop_status_e;

typedef enum {
  EVENT_TYPE_NONE = 0,
  EVENT_TYPE_IO = 0x00000001,
  EVENT_TYPE_TIMEOUT = 0x00000010,
  EVENT_TYPE_PERIOD = 0x00000020,
  EVENT_TYPE_TIMER = EVENT_TYPE_TIMEOUT | EVENT_TYPE_PERIOD,
  EVENT_TYPE_IDLE = 0x00000100,
  EVENT_TYPE_CUSTOM = 0x00000400, // 1024
} eevent_type_e;

#define eevent_set_id(ev, id)           ((eevent_t*)(ev))->event_id = id
#define eevent_set_cb(ev, cb)           ((eevent_t*)(ev))->cb = cb
#define eevent_set_priority(ev, prio)   ((eevent_t*)(ev))->priority = prio
#define eevent_set_userdata(ev, udata)  ((eevent_t*)(ev))->userdata = (void*)udata

#define eevent_loop(ev)         (((eevent_t*)(ev))->loop)
#define eevent_type(ev)         (((eevent_t*)(ev))->event_type)
#define eevent_id(ev)           (((eevent_t*)(ev))->event_id)
#define eevent_cb(ev)           (((eevent_t*)(ev))->cb)
#define eevent_priority(ev)     (((eevent_t*)(ev))->priority)
#define eevent_userdata(ev)     (((eevent_t*)(ev))->userdata)

#define EVENT_FLAGS        \
    unsigned    destroy :1; \
    unsigned    active  :1; \
    unsigned    pending :1;

#define EVENT_FIELDS                   \
    eloop_t*            loop;           \
    eevent_type_e        event_type;     \
    uint64_t            event_id;       \
    eevent_cb            cb;             \
    void*               userdata;       \
    void*               privdata;       \
    struct eevent_s*     pending_next;   \
    int                 priority;       \
    EVENT_FLAGS

struct eevent_s {
  EVENT_FIELDS
};

#endif //EVENT_ELOOP_H
