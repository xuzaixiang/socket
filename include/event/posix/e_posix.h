#ifndef EVENT_POSIX_H
#define EVENT_POSIX_H

#include <signal.h>
#include <sys/socket.h> // sockaddr
#include <netinet/in.h> // sockaddr_in sockaddr_in6
#include <arpa/inet.h> // inet_pton
#include <netdb.h> // gethostbyname

#include "e_fcntl.h"
#include "e_unistd.h"
#include "e_pthread.h"

#endif