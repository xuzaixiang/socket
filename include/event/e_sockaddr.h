//
// Created by 20123460 on 2022/1/15.
//

#ifndef EVENT__SOCKADDR_H
#define EVENT__SOCKADDR_H

#include "e_event.h"

typedef union {
  struct sockaddr     sa;
//  struct sockaddr_in  sin;
//  struct sockaddr_in6 sin6;
#ifdef ENABLE_UDS
  struct sockaddr_un  sun;
#endif
} sockaddr_u;


#endif //EVENT__SOCKADDR_H
