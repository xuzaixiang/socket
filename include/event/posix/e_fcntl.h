//
// Created by 20123460 on 2022/1/24.
//

#ifndef EVENT_FCNTL_H
#define EVENT_FCNTL_H

#include <fcntl.h> //fcntl

#define blocking(s)     fcntl(s, F_SETFL, fcntl(s, F_GETFL) & ~O_NONBLOCK)
#define nonblocking(s)  fcntl(s, F_SETFL, fcntl(s, F_GETFL) |  O_NONBLOCK)

#endif //EVENT_FCNTL_H
