//
// Created by 20123460 on 2022/1/7.
//

#ifndef EVENT_EVENT_H
#define EVENT_EVENT_H

#include "e_loop.h"
#include "e_list.h"

struct e_loop_s {
  uint32_t flags;
  e_loop_status_e status;
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
  struct list_head            idles;
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

struct e_idle_s {
//  EVENT_FIELDS
      uint32_t    repeat;
//private:
//  struct list_node node;
};

struct e_io_s {
//  HEVENT_FIELDS
//  // flags
//  unsigned    ready       :1;
//  unsigned    closed      :1;
//  unsigned    accept      :1;
//  unsigned    connect     :1;
//  unsigned    connectex   :1; // for ConnectEx/DisconnectEx
//  unsigned    recv        :1;
//  unsigned    send        :1;
//  unsigned    recvfrom    :1;
//  unsigned    sendto      :1;
//  unsigned    close       :1;
//  unsigned    alloced_readbuf :1; // for hio_alloc_readbuf
//// public:
//  hio_type_e  io_type;
//  uint32_t    id; // fd cannot be used as unique identifier, so we provide an id
//  int         fd;
//  int         error;
//  int         events;
//  int         revents;
//  struct sockaddr*    localaddr;
//  struct sockaddr*    peeraddr;
//  uint64_t            last_read_hrtime;
//  uint64_t            last_write_hrtime;
//  // read
//  fifo_buf_t          readbuf;
//  unsigned int        read_flags;
//  // for hio_read_until
//  union {
//    unsigned int    read_until_length;
//    unsigned char   read_until_delim;
//  };
//  uint32_t            small_readbytes_cnt; // for readbuf autosize
//  // write
//  struct write_queue  write_queue;
//  hrecursive_mutex_t  write_mutex; // lock write and write_queue
//  uint32_t            write_bufsize;
//  // callbacks
//  hread_cb    read_cb;
//  hwrite_cb   write_cb;
//  hclose_cb   close_cb;
//  haccept_cb  accept_cb;
//  hconnect_cb connect_cb;
//  // timers
//  int         connect_timeout;    // ms
//  int         close_timeout;      // ms
//  int         read_timeout;       // ms
//  int         write_timeout;      // ms
//  int         keepalive_timeout;  // ms
//  int         heartbeat_interval; // ms
//  hio_send_heartbeat_fn heartbeat_fn;
//  htimer_t*   connect_timer;
//  htimer_t*   close_timer;
//  htimer_t*   read_timer;
//  htimer_t*   write_timer;
//  htimer_t*   keepalive_timer;
//  htimer_t*   heartbeat_timer;
//  // upstream
//  struct hio_s*       upstream_io;    // for hio_setup_upstream
//  // unpack
//  unpack_setting_t*   unpack_setting; // for hio_set_unpack
//  // ssl
//  void*       ssl; // for hio_enable_ssl / hio_set_ssl
//  // context
//  void*       ctx; // for hio_context / hio_set_context
//// private:
//#if defined(EVENT_POLL) || defined(EVENT_KQUEUE)
//  int         event_index[2]; // for poll,kqueue
//#endif
//
//#ifdef EVENT_IOCP
//  void*       hovlp;          // for iocp/overlapio
//#endif
//
//#if WITH_RUDP
//  rudp_t          rudp;
//#if WITH_KCP
//    kcp_setting_t*  kcp_setting;
//#endif
//#endif
};

#endif //EVENT_EVENT_H
