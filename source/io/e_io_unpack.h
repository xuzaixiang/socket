//
// Created by 20123460 on 2022/2/8.
//

#ifndef EVENT_IO_UNPACK_H
#define EVENT_IO_UNPACK_H

#include <event/e_io.h>

int e_io_unpack(e_io_t* io, void* buf, int readbytes);
int e_io_unpack_by_fixed_length(e_io_t* io, void* buf, int readbytes);
int e_io_unpack_by_delimiter(e_io_t* io, void* buf, int readbytes);
int e_io_unpack_by_length_field(e_io_t* io, void* buf, int readbytes);

#endif // EVENT_IO_UNPACK_H
