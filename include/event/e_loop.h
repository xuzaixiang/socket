#ifndef EVENT_ELOOP_H
#define EVENT_ELOOP_H

#include "stdint.h"//C99

#include "e_export.h"
#include "e_platform.h"
#include "io/e_io.h"

#define E_LOWEST_PRIORITY    (-5)
#define E_HIGHEST_PRIORITY     5
#define E_PRIORITY_SIZE  (E_HIGHEST_PRIORITY-E_LOWEST_PRIORITY+1)
#define E_LOW_PRIORITY       (-3)
#define E_NORMAL_PRIORITY      0
#define E_HIGH_PRIORITY        3

typedef struct e_loop_s e_loop_t;
typedef struct e_event_s e_event_t;

typedef struct e_io_s e_io_t;

typedef void (*e_event_cb)(e_event_t *ev);
typedef void (*e_accept_cb)(e_io_t *io);

typedef enum {
  ELOOP_STATUS_STOP,
  ELOOP_STATUS_RUNNING,
  ELOOP_STATUS_PAUSE
} e_loop_status_e;


typedef enum {
  EVENT_TYPE_NONE = 0,
  EVENT_TYPE_IO = 0x00000001,
  EVENT_TYPE_TIMEOUT = 0x00000010,
  EVENT_TYPE_PERIOD = 0x00000020,
  EVENT_TYPE_TIMER = EVENT_TYPE_TIMEOUT | EVENT_TYPE_PERIOD,
  EVENT_TYPE_IDLE = 0x00000100,
  EVENT_TYPE_CUSTOM = 0x00000400, // 1024
} e_event_type_e;

#define e_event_set_id(ev, id)           ((e_event_t*)(ev))->event_id = id
#define e_event_set_cb(ev, cb)           ((e_event_t*)(ev))->cb = cb
#define e_event_set_priority(ev, prio)   ((e_event_t*)(ev))->priority = prio
#define e_event_set_userdata(ev, udata)  ((e_event_t*)(ev))->userdata = (void*)udata

#define e_event_loop(ev)         (((e_event_t*)(ev))->loop)
#define e_event_type(ev)         (((e_event_t*)(ev))->event_type)
#define e_event_id(ev)           (((e_event_t*)(ev))->event_id)
#define e_event_cb(ev)           (((e_event_t*)(ev))->cb)
#define e_event_priority(ev)     (((e_event_t*)(ev))->priority)
#define e_event_userdata(ev)     (((e_event_t*)(ev))->userdata)

#define EVENT_FLAGS        \
    unsigned    destroy :1; \
    unsigned    active  :1; \
    unsigned    pending :1;

#define EVENT_FIELDS                   \
    e_loop_t*            loop;           \
    e_event_type_e        event_type;     \
    uint64_t            event_id;       \
    e_event_cb            cb;             \
    void*               userdata;       \
    void*               privdata;       \
    struct e_event_s*     pending_next;   \
    int                 priority;       \
    EVENT_FLAGS

struct e_event_s {
  EVENT_FIELDS
};

#define ELOOP_FLAG_RUN_ONCE                     0x00000001
#define ELOOP_FLAG_AUTO_FREE                    0x00000002
#define ELOOP_FLAG_QUIT_WHEN_NO_ACTIVE_EVENTS   0x00000004
e_loop_t *e_loop_new(int flags DEFAULT(ELOOP_FLAG_AUTO_FREE));


e_io_t *e_loop_create_tcp_server(e_loop_t *loop, const char *host, int port, e_accept_cb accept_cb);

#endif //EVENT_ELOOP_H
