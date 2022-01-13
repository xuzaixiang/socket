#ifndef EVENT_IO_TYPE_H
#define EVENT_IO_TYPE_H

#include <stdbool.h>
#include "e_export.h"
#include "e_event.h"
#include "e_unpack.h"
#include "e_socket.h"
#include "e_buf.h"
#include "e_queue.h"
#include "e_ssl.h"
#include "e_error.h"

// e_io_read_flags
#define EVENT_IO_READ_ONCE           0x1
#define EVENT_IO_READ_UNTIL_LENGTH   0x2
#define EVENT_IO_READ_UNTIL_DELIM    0x4

#define EVENT_IO_DEFAULT_CONNECT_TIMEOUT     5000    // ms
#define EVENT_IO_DEFAULT_CLOSE_TIMEOUT       60000   // ms
#define EVENT_IO_DEFAULT_KEEPALIVE_TIMEOUT   75000   // ms
#define EVENT_IO_DEFAULT_HEARTBEAT_INTERVAL  10000   // ms

typedef void (*e_io_cb)(e_io_t *io);
typedef void (*e_io_send_heartbeat_fn)(e_io_t *io);

EVENT_QUEUE_DECL(offset_buf_t, write_queue);

typedef enum {
  EVENT_IO_TYPE_UNKNOWN = 0,
  EVENT_IO_TYPE_STDIN = 0x00000001,//  0000 0001
  EVENT_IO_TYPE_STDOUT = 0x00000002,//  0000 0010
  EVENT_IO_TYPE_STDERR = 0x00000004,//  0000 0100
  EVENT_IO_TYPE_STDIO = 0x0000000F,//  0000 1111

  EVENT_IO_TYPE_FILE = 0x00000010,//  0001 0000

  EVENT_IO_TYPE_IP = 0x00000100,//  0001 0000 0000
  EVENT_IO_TYPE_SOCK_RAW = 0x00000F00,//  1111 0000 0000

  EVENT_IO_TYPE_UDP = 0x00001000,//  0000 0001 0000 0000 0000
  EVENT_IO_TYPE_KCP = 0x00002000,//  0000 0010 0000 0000 0000
  EVENT_IO_TYPE_DTLS = 0x00010000,//  0001 0000 0000 0000 0000
  EVENT_IO_TYPE_SOCK_DGRAM = 0x000FF000,//  1111 1111 0000 0000 0000

  EVENT_IO_TYPE_TCP = 0x00100000,
  EVENT_IO_TYPE_SSL = 0x01000000,
  EVENT_IO_TYPE_TLS = EVENT_IO_TYPE_SSL,
  EVENT_IO_TYPE_SOCK_STREAM = 0x0FF00000,

  EVENT_IO_TYPE_SOCKET = 0x0FFFFF00,
} e_io_type_e;

typedef enum {
  EVENT_IO_SERVER_SIDE = 0,
  EVENT_IO_CLIENT_SIDE = 1,
} e_io_side_e;

struct e_io_s {
  EVENT_FIELDS
  // flags
  unsigned ready: 1;
  unsigned closed: 1;
  unsigned accept: 1;
  unsigned connect: 1;
  unsigned connectex: 1; // for ConnectEx/DisconnectEx
  unsigned recv: 1;
  unsigned send: 1;
  unsigned recvfrom: 1;
  unsigned sendto: 1;
  unsigned close: 1;
  unsigned alloced_readbuf: 1; // for hio_alloc_readbuf
// public:
  e_io_type_e io_type;
  uint32_t id; // fd cannot be used as unique identifier, so we provide an id
  int fd;
  int error;
  int events;
  int revents;
  struct sockaddr *localaddr;
  struct sockaddr *peeraddr;
  uint64_t last_read_hrtime;
  uint64_t last_write_hrtime;
  // read
  fifo_buf_t readbuf;
  unsigned int read_flags;
  // for hio_read_until
  union {
    unsigned int read_until_length;
    unsigned char read_until_delim;
  };
  uint32_t small_readbytes_cnt; // for readbuf autosize
  // write
  struct write_queue write_queue;
  e_recursive_mutex_t write_mutex; // lock write and write_queue
  uint32_t write_bufsize;
  // callbacks
  e_read_cb read_cb;
  e_write_cb write_cb;
  e_close_cb close_cb;
  e_accept_cb accept_cb;
  e_connect_cb connect_cb;
  // timers
  int connect_timeout;    // ms
  int close_timeout;      // ms
  int read_timeout;       // ms
  int write_timeout;      // ms
  int keepalive_timeout;  // ms
  int heartbeat_interval; // ms
  e_io_send_heartbeat_fn heartbeat_fn;
  e_timer_t *connect_timer;
  e_timer_t *close_timer;
  e_timer_t *read_timer;
  e_timer_t *write_timer;
  e_timer_t *keepalive_timer;
  e_timer_t *heartbeat_timer;
  // upstream
  struct e_io_s *upstream_io;    // for hio_setup_upstream
  // unpack
  unpack_setting_t *unpack_setting; // for hio_set_unpack
  // ssl
  void *ssl; // for hio_enable_ssl / hio_set_ssl
  // context
  void *ctx; // for hio_context / hio_set_context
// private:
#if defined(EVENT_POLL) || defined(EVENT_KQUEUE)
  int         event_index[2]; // for poll,kqueue
#endif

#ifdef EVENT_IOCP
  void*       hovlp;          // for iocp/overlapio
#endif

#if WITH_RUDP
  rudp_t          rudp;
#if WITH_KCP
    kcp_setting_t*  kcp_setting;
#endif
#endif
};

int hio_close_async(e_io_t *io);

int e_io_read_remain(e_io_t *io);

int e_io_del(e_io_t *io, int events DEFAULT(EVENT_RDWR));
int e_io_add(e_io_t *io, e_io_cb cb, int events DEFAULT(EVENT_READ));

e_io_t *e_io_get(e_loop_t *loop, int fd);


// @hio_create_socket: socket -> bind -> listen
// sockaddr_set_ipport -> socket -> hio_get(loop, sockfd) ->
// side == HIO_SERVER_SIDE ? bind ->
// type & HIO_TYPE_SOCK_STREAM ? listen ->
e_io_t *e_io_create_socket(e_loop_t *loop, const char *host, int port,
                           e_io_type_e type DEFAULT(EVENT_IO_TYPE_TCP),
                           e_io_side_e side DEFAULT(EVENT_IO_SERVER_SIDE));

void e_io_set_localaddr(e_io_t *io, struct sockaddr *addr, int addrlen);
void e_io_set_peeraddr(e_io_t *io, struct sockaddr *addr, int addrlen);

// set callbacks
void e_io_setcb_close(e_io_t *io, e_close_cb close_cb);
void e_io_setcb_accept(e_io_t *io, e_accept_cb accept_cb);
void e_io_setcb_connect(e_io_t *io, e_connect_cb connect_cb);
void e_io_setcb_read(e_io_t *io, e_read_cb read_cb);
void e_io_setcb_write(e_io_t *io, e_write_cb write_cb);
// get callbacks
e_accept_cb e_io_getcb_accept(e_io_t *io);
e_connect_cb e_io_getcb_connect(e_io_t *io);
e_read_cb e_io_getcb_read(e_io_t *io);
e_write_cb e_io_getcb_write(e_io_t *io);
e_close_cb e_io_getcb_close(e_io_t *io);
// call callbacks
void e_io_accept_cb(e_io_t *io);
void e_io_connect_cb(e_io_t *io);
void e_io_handle_read(e_io_t *io, void *buf, int readbytes);
void e_io_read_cb(e_io_t *io, void *buf, int len);
void e_io_write_cb(e_io_t *io, const void *buf, int len);
void e_io_close_cb(e_io_t *io);

// timer
void e_io_del_connect_timer(e_io_t *io);
void e_io_del_close_timer(e_io_t *io);
void e_io_del_read_timer(e_io_t *io);
void e_io_del_write_timer(e_io_t *io);
void e_io_del_keepalive_timer(e_io_t *io);
void e_io_del_heartbeat_timer(e_io_t *io);

// readbuf
bool e_io_is_loop_readbuf(e_io_t *io);
bool e_io_is_alloced_readbuf(e_io_t *io);
void e_io_free_readbuf(e_io_t *io);
void e_io_alloc_readbuf(e_io_t *io, int len);

// fields
// NOTE: fd cannot be used as unique identifier, so we provide an id.
uint32_t e_io_id(e_io_t *io);
int e_io_fd(e_io_t *io);
int e_io_error(e_io_t *io);
int e_io_events(e_io_t *io);
int e_io_revents(e_io_t *io);
e_io_type_e e_io_type(e_io_t *io);
struct sockaddr *e_io_localaddr(e_io_t *io);
struct sockaddr *e_io_peeraddr(e_io_t *io);
void e_io_set_context(e_io_t *io, void *ctx);
void *e_io_context(e_io_t *io);
bool e_io_is_opened(e_io_t *io);
bool e_io_is_closed(e_io_t *io);
fifo_buf_t *e_io_get_readbuf(e_io_t *io);
size_t e_io_write_bufsize(e_io_t *io);
bool e_io_write_queue_is_empty(e_io_t *io);
uint64_t e_io_last_read_time(e_io_t *io);  // ms
uint64_t e_io_last_write_time(e_io_t *io);  // ms

#define e_io_read_start(io) e_io_read(io)
#define e_io_read_stop(io)  e_io_del(io, EVENT_READ)

#endif
