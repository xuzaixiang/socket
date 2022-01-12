//
// Created by 20123460 on 2022/1/7.
//

#ifndef EVENT_EVENT_H
#define EVENT_EVENT_H

#include <stdint.h>//c99

#include "e_list.h"
#include "e_array.h"
#include "e_api.h"
#include "e_heap.h"

typedef struct e_loop_s e_loop_t;
typedef struct e_io_s e_io_t;
typedef struct e_event_s e_event_t;
typedef struct e_timer_s     e_timer_t;
typedef struct e_timeout_s   e_timeout_t;
typedef struct e_period_s    e_period_t;

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

#define e_event_priority(ev)     (((e_event_t*)(ev))->priority)
#define e_event_userdata(ev)     (((e_event_t*)(ev))->userdata)

#define e_event_cb(ev)           (((e_event_t*)(ev))->cb)

typedef void (*e_event_cb)(e_event_t *ev);
//typedef void (*e_idle_cb)(e_idle_t *idle);
typedef void (*e_timer_cb)(e_timer_t *timer);
typedef void (*e_io_cb)(e_io_t *io);

typedef void (*e_accept_cb)(e_io_t *io);
typedef void (*e_connect_cb)(e_io_t *io);
typedef void (*e_read_cb)(e_io_t *io, void *buf, int readbytes);
typedef void (*e_write_cb)(e_io_t *io, const void *buf, int writebytes);
typedef void (*e_close_cb)(e_io_t *io);

typedef struct TIMER_FIELDS TIMER_FIELDS;
#define EVENT_READ  0x0001
#define EVENT_WRITE 0x0004
#define EVENT_RDWR  (EVENT_READ|EVENT_WRITE)

#define EVENT_LOWEST_PRIORITY    (-5)
#define EVENT_HIGHEST_PRIORITY     5
#define EVENT_LOW_PRIORITY       (-3)
#define EVENT_NORMAL_PRIORITY      0
#define EVENT_HIGH_PRIORITY        3
#define EVENT_PRIORITY_SIZE  (EVENT_HIGHEST_PRIORITY-EVENT_LOWEST_PRIORITY+1)
#define EVENT_PRIORITY_INDEX(priority) (priority-EVENT_LOWEST_PRIORITY)

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
    void*                 privdata;       \
    struct e_event_s*     pending_next;   \
    int                   priority;       \
    EVENT_FLAGS

struct e_event_s {
  EVENT_FIELDS
};

struct e_idle_s {
  EVENT_FIELDS
  uint32_t repeat;
//private:
  struct list_node node;
};

#define TIMER_FIELDS                   \
    EVENT_FIELDS                       \
    uint32_t    repeat;                 \
    uint64_t    next_timeout;           \
    struct heap_node node;

struct e_timer_s {
  TIMER_FIELDS
};

struct e_timeout_s {
  TIMER_FIELDS
      uint32_t    timeout;                \
};

struct e_period_s {
  TIMER_FIELDS
      int8_t      minute;
  int8_t      hour;
  int8_t      day;
  int8_t      week;
  int8_t      month;
};

#define EVENT_ENTRY(p)          container_of(p, e_event_t, pending_node)
#define IDLE_ENTRY(p)           container_of(p, e_idle_t,  node)
#define TIMER_ENTRY(p)          container_of(p, e_timer_t, node)

uint64_t e_loop_next_event_id();

#define EVENT_ACTIVE(ev) \
    if (!ev->active) {\
        ev->active = 1;\
        ev->loop->nactives++;\
    }\

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

#define EVENT_ADD(loop, ev, cb) \
    do {\
        ev->loop = loop;\
        ev->event_id = e_loop_next_event_id();\
        ev->cb = (e_event_cb)cb;\
        EVENT_ACTIVE(ev);\
    } while(0)

#define EVENT_INACTIVE(ev) \
    if (ev->active) {\
        ev->active = 0;\
        ev->loop->nactives--;\
    }

#define EVENT_DEL(ev) \
    do {\
        EVENT_INACTIVE(ev);\
        if (!ev->pending) {\
            EVENT_FREE(ev);\
        }\
    } while(0)
#endif //EVENT_EVENT_H
