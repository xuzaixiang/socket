#ifndef EVENT_ELOOP_H
#define EVENT_ELOOP_H

#include "stdint.h"//C99

#include "e_export.h"
#include "e_platform.h"
#include "e_io.h"
#include "e_list.h"
#include "e_array.h"
#include "e_queue.h"
#include "e_buf.h"
#include "e_heap.h"

// loop
#define EVENT_LOOP_FLAG_RUN_ONCE                     0x00000001
#define EVENT_LOOP_FLAG_AUTO_FREE                    0x00000002
#define EVENT_LOOP_FLAG_QUIT_WHEN_NO_ACTIVE_EVENTS   0x00000004

EVENT_ARRAY_DECL(e_io_t*, io_array)
EVENT_QUEUE_DECL(e_event_t, e_queue)

typedef struct e_loop_s e_loop_t;
typedef struct e_event_s e_event_t;
typedef struct e_idle_s e_idle_t;
typedef struct e_io_s e_io_t;

typedef void (*e_event_cb)(e_event_t *ev);
typedef void (*e_accept_cb)(e_io_t *io);

typedef enum {
  EVENT_LOOP_STATUS_STOP,
  EVENT_LOOP_STATUS_RUNNING,
  EVENT_LOOP_STATUS_PAUSE
} e_loop_status_e;

struct e_loop_s {
  uint32_t flags;
  e_loop_status_e status;
  uint64_t start_ms;       // ms
  uint64_t start_hrtime;   // us
  uint64_t end_hrtime;
  uint64_t cur_hrtime;
  uint64_t loop_cnt;
  long pid;
  long tid;
  void *userdata;

//private:
  // events
  uint32_t intern_nevents;
  uint32_t nactives;
  uint32_t npendings;
  // pendings: with priority as array.index
  e_event_t *pendings[EVENT_PRIORITY_SIZE];
  // idles
  struct list_head idles;
  uint32_t nidles;
  // timers
  struct heap timers;
  uint32_t ntimers;
  // ios: with fd as array.index
  struct io_array ios;
  uint32_t nios;
  // one loop per thread, so one readbuf per loop is OK.
  e_buf_t readbuf;
  void *iowatcher;
  // custom_events
  int sockpair[2];
  e_queue custom_events;
  e_mutex_t custom_events_mutex;
};
typedef struct e_loop_s e_loop_t;

#define ELOOP_FLAG_RUN_ONCE                     0x00000001
#define ELOOP_FLAG_AUTO_FREE                    0x00000002
#define ELOOP_FLAG_QUIT_WHEN_NO_ACTIVE_EVENTS   0x00000004

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// timer
/*
 * minute   hour    day     week    month       cb
 * 0~59     0~23    1~31    0~6     1~12
 *  -1      -1      -1      -1      -1          cron.minutely
 *  30      -1      -1      -1      -1          cron.hourly
 *  30      1       -1      -1      -1          cron.daily
 *  30      1       15      -1      -1          cron.monthly
 *  30      1       -1       5      -1          cron.weekly
 *  30      1        1      -1      10          cron.yearly
 */
e_timer_t *e_timer_add_period(e_loop_t *loop, e_timer_cb cb,
                              int8_t minute DEFAULT(0), int8_t hour  DEFAULT(-1), int8_t day DEFAULT(-1),
                              int8_t week   DEFAULT(-1), int8_t month DEFAULT(-1), uint32_t repeat DEFAULT(INFINITE));
e_timer_t *e_timer_add(e_loop_t *loop, e_timer_cb cb, uint32_t timeout, uint32_t repeat DEFAULT(INFINITE));
void e_timer_del(e_timer_t *timer);
void e_timer_reset(e_timer_t *timer);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// idle
e_idle_t *e_idle_add(e_loop_t *loop, e_idle_cb cb, uint32_t repeat DEFAULT(INFINITE));
void e_idle_del(e_idle_t *idle);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// custom_event
/*
 * e_event_t ev;
 * memset(&ev, 0, sizeof(e_event_t));
 * ev.event_type = (e_event_type_e)(EVENT_TYPE_CUSTOM + 1);
 * ev.cb = custom_event_cb;
 * ev.userdata = userdata;
 * e_loop_post_event(loop, &ev);
 */
void e_loop_post_event(e_loop_t *loop, e_event_t *ev);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// userdata
void e_loop_set_userdata(e_loop_t *loop, void *userdata);
void *e_loop_userdata(e_loop_t *loop);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int e_loop_create_sockpair(e_loop_t *loop);

static void __e_timer_del(e_timer_t *timer);
static void __e_idle_del(e_idle_t *idle);

e_loop_t *e_loop_new(int flags DEFAULT(ELOOP_FLAG_AUTO_FREE));
void e_loop_free(e_loop_t **pp);
int e_loop_run(e_loop_t *loop);

uint64_t e_loop_now(e_loop_t *loop);          // s
uint64_t e_loop_now_ms(e_loop_t *loop);       // ms
uint64_t e_loop_now_hrtime(e_loop_t *loop);   // us
void e_loop_update_time(e_loop_t *loop);

e_io_t *e_read(e_loop_t *loop, int fd, void *buf, size_t len, e_read_cb read_cb);
e_io_t *e_write(e_loop_t *loop, int fd, const void *buf, size_t len, e_write_cb write_cb DEFAULT(NULL));
void e_close(e_loop_t *loop, int fd);

e_io_t *e_loop_create_tcp_server(e_loop_t *loop, const char *host, int port, e_accept_cb accept_cb);

#endif //EVENT_ELOOP_H
