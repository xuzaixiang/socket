//
// Created by 20123460 on 2022/1/15.
//

#ifndef EVENT_SOCKET_H
#define EVENT_SOCKET_H

#include "event/e_io.h"
#include "event/e_event.h"

typedef struct e_loop_s e_loop_t;

e_io_t *e_socket_create(e_loop_t *loop, const char *host, int port, e_io_type_t type DEFAULT(EVENT_IO_TYPE_TCP),
                        e_io_side_t side DEFAULT(EVENT_IO_SERVER_SIDE));
int e_socket_errno();

void e_socket_init(e_io_t* io);
e_io_type_t e_socket_type(int fd);

#endif //EVENT_SOCKET_H
