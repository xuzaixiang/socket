//
// Created by 20123460 on 2022/1/7.
//

#ifndef EVENT_EVENT_H
#define EVENT_EVENT_H

#include "event/eloop.h"

struct eloop_s {
  uint32_t flags;
  eloop_status_e status;
  uint64_t start_ms;       // ms
  uint64_t start_hrtime;   // us
  uint64_t end_hrtime;
  uint64_t cur_hrtime;
  uint64_t loop_cnt;
  long pid;
  long tid;
//  void *userdata;

//private:
  // events
//  uint32_t                    intern_nevents;
//  uint32_t                    nactives;
//  uint32_t                    npendings;
  // pendings: with priority as array.index
//  eevent_t*                   pendings[EEVENT_PRIORITY_SIZE];
  // idles
//  struct list_head            idles;
//  uint32_t                    nidles;
//  // timers
//  struct heap                 timers;
//  uint32_t                    ntimers;
//  // ios: with fd as array.index
//  struct io_array             ios;
//  uint32_t                    nios;
//  // one loop per thread, so one readbuf per loop is OK.
//  hbuf_t                      readbuf;
//  void*                       iowatcher;
//  // custom_events
//  int                         sockpair[2];
//  event_queue                 custom_events;
//  hmutex_t                    custom_events_mutex;
};

//struct eio_s {
//
//}

#endif //EVENT_EVENT_H
