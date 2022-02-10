//
// Created by 20123460 on 2022/2/9.
//

#ifndef EVENT_IO_H
#define EVENT_IO_H

#include "e_def.h"

typedef struct e_io_s e_io_t;
typedef struct e_loop_s e_loop_t;
typedef struct e_event_s e_event_t;

typedef void (*e_read_cb)(e_io_t *io);
typedef void (*e_write_cb)(e_io_t *io);
typedef void (*e_io_cb)(e_io_t *io);
typedef void (*e_event_cb)(e_event_t *ev);
BEGIN_EXTERN_C

EVENT_EXPORT e_io_t *e_io_get(e_loop_t *loop, int fd);
EVENT_EXPORT int e_io_add(e_io_t *io, e_io_cb cb, int events);
EVENT_EXPORT int e_io_read(e_io_t *io, e_read_cb read_cb);
EVENT_EXPORT void e_io_free(e_io_t *io);

END_EXTERN_C

#endif // EVENT_IO_H
