//
// Created by 20123460 on 2022/1/7.
//
#ifndef EVENT_IO_WATCHER_H
#define EVENT_IO_WATCHER_H

#include "eloop.h"
#include "eplatform.h"

#if !defined(EVENT_SELECT) &&   \
    !defined(EVENT_POLL) &&     \
    !defined(EVENT_EPOLL) &&    \
    !defined(EVENT_KQUEUE) &&   \
    !defined(EVENT_IOCP) &&     \
    !defined(EVENT_PORT) &&     \
    !defined(EVENT_NOEVENT)
#ifdef SNF_OS_WIN
// #define EVENT_IOCP // IOCP improving
#define EVENT_POLL
#elif defined(SNF_OS_LINUX)
#define EVENT_EPOLL
#elif defined(SNF_OS_MAC)
#define EVENT_KQUEUE
#elif defined(SNF_OS_BSD)
#define EVENT_KQUEUE
#elif defined(SNF_OS_SOLARIS)
#define EVENT_PORT
#else
#define EVENT_SELECT
#endif
#endif

int iowatcher_init(eloop_t* loop);
int iowatcher_cleanup(eloop_t* loop);
int iowatcher_add_event(eloop_t* loop, int fd, int events);
int iowatcher_del_event(eloop_t* loop, int fd, int events);
int iowatcher_poll_events(eloop_t* loop, int timeout);

#endif //EVENT_IO_WATCHER_H
