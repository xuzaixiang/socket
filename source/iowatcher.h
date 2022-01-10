//
// Created by 20123460 on 2022/1/7.
//
#ifndef EVENT_IO_WATCHER_H
#define EVENT_IO_WATCHER_H

#include "event/eloop.h"
#include "eplatform.h"

int iowatcher_init(eloop_t* loop);
int iowatcher_cleanup(eloop_t* loop);
int iowatcher_add_event(eloop_t* loop, int fd, int events);
int iowatcher_del_event(eloop_t* loop, int fd, int events);
int iowatcher_poll_events(eloop_t* loop, int timeout);

#endif //EVENT_IO_WATCHER_H
