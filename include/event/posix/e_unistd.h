#ifndef EVENT_UNISTD_H
#define EVENT_UNISTD_H

#include <unistd.h>
#define e_getpid   (long)getpid

#define closesocket(fd) close(fd)
#endif