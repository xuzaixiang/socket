#ifndef EVENT_FCNTL_H
#define EVENT_FCNTL_H

#include <fcntl.h>

#define blocking(s)     fcntl(s, F_SETFL, fcntl(s, F_GETFL) & ~O_NONBLOCK)
#define nonblocking(s)  fcntl(s, F_SETFL, fcntl(s, F_GETFL) |  O_NONBLOCK)

#endif