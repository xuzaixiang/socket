//
// Created by 20123460 on 2022/1/13.
//

#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include "e_event.h"
#include "e_io.h"

EVENT_ARRAY_DECL(e_io_t*, io_array)
#define EVENT_IO_ARRAY_INIT_SIZE              1024

typedef void (*e_accept_cb)(e_io_t *io);

typedef enum e_loop_status_e {
  EVENT_LOOP_STATUS_STOP,
  EVENT_LOOP_STATUS_RUNNING,
  EVENT_LOOP_STATUS_PAUSE
} e_loop_status_t;

typedef struct e_loop_s {
  uint32_t flags;
  e_loop_status_t status;
  uint64_t loop_cnt;
  long pid;
  long tid;
  void *userdata;

  // ios: with fd as array.index
  struct io_array ios;
  uint32_t nios;

  // events
  uint32_t intern_nevents;
  uint32_t nactives;// io active num
  uint32_t npendings;
  // pendings: with priority as array.index
  e_event_t *pendings[EVENT_PRIORITY_SIZE];

  // one loop per thread, so one readbuf per loop is OK.
  e_buf_t                      readbuf;
  void *iowatcher;
} e_loop_t;

#define EVENT_LOOP_FLAG_RUN_ONCE                     0x00000001
#define EVENT_LOOP_FLAG_AUTO_FREE                    0x00000002
#define EVENT_LOOP_FLAG_QUIT_WHEN_NO_ACTIVE_EVENTS   0x00000004

EVENT_EXPORT e_loop_t *e_loop_new(uint32_t flags DEFAULT(EVENT_LOOP_FLAG_AUTO_FREE));
EVENT_EXPORT int e_loop_run(e_loop_t *loop);
EVENT_EXPORT void e_loop_free(e_loop_t **pp);

/*
 * hevent_t ev;
 * memset(&ev, 0, sizeof(hevent_t));
 * ev.event_type = (hevent_type_e)(HEVENT_TYPE_CUSTOM + 1);
 * ev.cb = custom_event_cb;
 * ev.userdata = userdata;
 * hloop_post_event(loop, &ev);
 */
// NOTE: hloop_post_event is thread-safe, used to post event from other thread to loop thread.
EVENT_EXPORT void e_loop_post_event(e_loop_t* loop, e_event_t* ev);
// server
EVENT_EXPORT e_io_t *e_loop_create_tcp_server(e_loop_t *loop, const char *host, int port, e_accept_cb accept_cb);
EVENT_EXPORT e_io_t *e_loop_create_udp_server(e_loop_t *loop, const char *host, int port);
EVENT_EXPORT e_io_t* e_loop_create_udp_client (e_loop_t* loop, const char* host, int port);
#endif //EVENT_LOOP_H
