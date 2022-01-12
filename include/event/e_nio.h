//
// Created by 20123460 on 2022/1/12.
//

#ifndef EVENT_NIO_H
#define EVENT_NIO_H

#include "e_api.h"

typedef struct e_io_s e_io_t;

int e_io_write(e_io_t *io, const void *buf, size_t len);
int e_io_close(e_io_t *io);
int e_io_read(e_io_t *io);
int e_io_connect(e_io_t* io);
int e_io_accept(e_io_t *io);

#endif //EVENT_NIO_H
