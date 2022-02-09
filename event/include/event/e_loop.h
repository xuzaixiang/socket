//
// Created by 20123460 on 2022/2/8.
//

#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include "e_alloc.h"
#include "e_def.h"
#include "e_include.h"
#include "e_thread.h"


#define EVENT_LOOP_FLAG_RUN_ONCE 0x00000001
#define EVENT_LOOP_FLAG_AUTO_FREE 0x00000002
#define EVENT_LOOP_FLAG_QUIT_WHEN_NO_ACTIVE_EVENTS 0x00000004

typedef struct e_loop_s e_loop_t;
typedef struct e_event_s e_event_t;

typedef void (*e_event_cb)(e_event_t *ev);

typedef enum e_loop_status_e {
  EVENT_LOOP_STATUS_STOP,
  EVENT_LOOP_STATUS_RUNNING,
  EVENT_LOOP_STATUS_PAUSE
} e_loop_status_t;

typedef enum e_event_type_e {
  EVENT_TYPE_NONE = 0,
  EVENT_TYPE_IO = 0x00000001,
  EVENT_TYPE_TIMEOUT = 0x00000010,
  EVENT_TYPE_PERIOD = 0x00000020,
  EVENT_TYPE_TIMER = EVENT_TYPE_TIMEOUT | EVENT_TYPE_PERIOD,
  EVENT_TYPE_IDLE = 0x00000100,
  EVENT_TYPE_CUSTOM = 0x00000400, // 1024
} e_event_type_t;

#define EVENT_FLAGS                                                            \
  unsigned destroy : 1;                                                        \
  unsigned active : 1;                                                         \
  unsigned pending : 1;

#define EVENT_FIELDS                                                           \
  e_loop_t *loop;                                                              \
  e_event_type_t event_type;                                                   \
  uint64_t event_id;                                                           \
  e_event_cb cb;                                                               \
  void *userdata;                                                              \
  void *privdata;                                                              \
  struct e_event_s *pending_next;                                              \
  int priority;                                                                \
  EVENT_FLAGS

// sizeof(struct e_event_s)=64 on x64
struct e_event_s {
  EVENT_FIELDS
};

BEGIN_EXTERN_C

EVENT_EXPORT e_loop_t *e_loop_new(int flags);
// WARN: Forbid to call {@link e_loop_free} if EVENT_LOOP_FLAG_AUTO_FREE set.
EVENT_EXPORT void e_loop_free(e_loop_t **pp);
EVENT_EXPORT int e_loop_run(e_loop_t* loop);

END_EXTERN_C

#endif // EVENT_LOOP_H
