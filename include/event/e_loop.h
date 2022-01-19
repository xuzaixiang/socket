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
  long pid;
  long tid;
  void *userdata;

  // ios: with fd as array.index
  struct io_array ios;
  uint32_t nios;

  void *iowatcher;
} e_loop_t;

#define EVENT_LOOP_FLAG_RUN_ONCE                     0x00000001
#define EVENT_LOOP_FLAG_AUTO_FREE                    0x00000002
#define EVENT_LOOP_FLAG_QUIT_WHEN_NO_ACTIVE_EVENTS   0x00000004

EVENT_EXPORT e_loop_t *e_loop_new(uint32_t flags DEFAULT(EVENT_LOOP_FLAG_AUTO_FREE));

// server
EVENT_EXPORT e_io_t *e_loop_create_tcp_server(e_loop_t *loop, const char *host, int port, e_accept_cb accept_cb);


#endif //EVENT_LOOP_H
