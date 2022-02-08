//
// Created by 20123460 on 2022/2/8.
//

#ifndef EVENT_IO_CB_H
#define EVENT_IO_CB_H

#include <event/e_io.h>

// callback - call
void e_io_accept_cb(e_io_t *io);
void e_io_connect_cb(e_io_t *io);
void e_io_write_cb(e_io_t *io, const void *buf, int len);
void e_io_read_cb(e_io_t *io, void *buf, int len);

#endif // EVENT_IO_CB_H
