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

#if defined(EVENT_OS_WIN)
#else
#include "posix/e_posix.h"
#if defined(EVENT_OS_LINUX)
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in sockaddr_in6
#include <arpa/inet.h> // inet_pton
#include <netdb.h> // gethostbyname
#else

#endif

typedef struct e_event_s     e_event_t;
typedef void (*e_event_cb)   (e_event_t* ev);

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
    void*                 privdata;       \
    struct hevent_s*      pending_next;   \
    int                   priority;       \
    EVENT_FLAGS

struct e_event_s {
  EVENT_FIELDS
};


#endif

#endif //EVENT_EVENT_H