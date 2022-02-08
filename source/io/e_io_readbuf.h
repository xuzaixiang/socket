//
// Created by 20123460 on 2022/2/8.
//

#ifndef EVENT_IO_READBUF_H
#define EVENT_IO_READBUF_H

#include <event/e_io.h>

// readbuf
void e_io_free_readbuf(e_io_t* io);
void e_io_alloc_readbuf(e_io_t *io, int len);
bool e_io_is_loop_readbuf(e_io_t *io);
bool e_io_is_alloced_readbuf(e_io_t *io);

#endif // EVENT_IO_READBUF_H
