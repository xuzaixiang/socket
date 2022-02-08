//
// Created by 20123460 on 2022/2/2.
//

#ifndef EVENT_NIO_H
#define EVENT_NIO_H

#include "../e_socket.h"
#include "../io/e_io_handle.h"
#include <event/e_io.h>

void e_nio_accept(e_io_t *io);
void e_nio_read(e_io_t *io);
void e_nio_write(e_io_t *io);
void e_nio_connect(e_io_t *io);

#endif // EVENT_NIO_H
