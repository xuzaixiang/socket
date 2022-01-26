#ifndef EVENT_IO_H
#define EVENT_IO_H

#include "e_event.h"
#include "e_buf.h"
#include "e_sockaddr.h"

// e_io_read_flags
#define EVENT_IO_READ_ONCE           0x1
#define EVENT_IO_READ_UNTIL_LENGTH   0x2
#define EVENT_IO_READ_UNTIL_DELIM    0x4

typedef struct e_io_s e_io_t;
// callback
typedef void (*e_io_cb)(e_io_t *io);
typedef void (*e_accept_cb)(e_io_t *io);
typedef void (*e_read_cb)(e_io_t *io, void *buf, int readbytes);
typedef void (*e_connect_cb)(e_io_t *io);
typedef void (*e_write_cb)(e_io_t *io, const void *buf, int writebytes);

EVENT_QUEUE_DECL(offset_buf_t, write_queue)

typedef enum e_io_type_e {
  EVENT_IO_TYPE_UNKNOWN = 0,
  EVENT_IO_TYPE_STDIN = 0x00000001,
  EVENT_IO_TYPE_STDOUT = 0x00000002,
  EVENT_IO_TYPE_STDERR = 0x00000004,
  EVENT_IO_TYPE_STDIO = 0x0000000F,

  EVENT_IO_TYPE_FILE = 0x00000010,

  EVENT_IO_TYPE_IP = 0x00000100,
  EVENT_IO_TYPE_SOCK_RAW = 0x00000F00,

  EVENT_IO_TYPE_UDP = 0x00001000,
  EVENT_IO_TYPE_KCP = 0x00002000,
  EVENT_IO_TYPE_DTLS = 0x00010000,
  EVENT_IO_TYPE_SOCK_DGRAM = 0x000FF000,

  EVENT_IO_TYPE_TCP = 0x00100000,
  EVENT_IO_TYPE_SSL = 0x01000000,
  EVENT_IO_TYPE_TLS = EVENT_IO_TYPE_SSL,
  EVENT_IO_TYPE_SOCK_STREAM = 0x0FF00000,

  EVENT_IO_TYPE_SOCKET = 0x0FFFFF00,
} e_io_type_t;

typedef enum e_io_side_e {
  EVENT_IO_SERVER_SIDE = 0,
  EVENT_IO_CLIENT_SIDE = 1,
} e_io_side_t;

struct e_io_s {
  EVENT_FIELDS
  unsigned ready: 1;
  unsigned closed: 1;
  unsigned accept: 1;
  unsigned connect: 1;
  unsigned close: 1;
  unsigned    alloced_readbuf :1; // for hio_alloc_readbuf
  // public:
  e_io_type_t io_type;
  uint32_t id; // fd cannot be used as unique identifier, so we provide an id
  int fd;
  int error;
  int events;
  int revents;
  struct sockaddr *localaddr;
  struct sockaddr *peeraddr;

  // write
  struct write_queue write_queue;
  e_recursive_mutex_t write_mutex; // lock write and write_queue
  uint32_t write_bufsize;

#if defined(EVENT_OS_MAC)
  int event_index[2]; // for poll,kqueue
#endif

// read
  fifo_buf_t readbuf;
  unsigned int read_flags;
  // for hio_read_until
  union {
    unsigned int read_until_length;
    unsigned char read_until_delim;
  };
  uint32_t small_readbytes_cnt; // for readbuf autosize

  // callback
  e_read_cb read_cb;
  e_accept_cb accept_cb;
  e_connect_cb connect_cb;
  e_write_cb write_cb;
};

// nio
// e_io_add(io, EVENT_READ) => accept => e_accept_cb
EVENT_EXPORT int e_io_accept(e_io_t *io);

EVENT_EXPORT e_io_t *e_io_get(e_loop_t *loop, int fd);
EVENT_EXPORT int e_io_add(e_io_t *io, e_io_cb cb, int events DEFAULT(EVENT_READ));
EVENT_EXPORT void e_io_init(e_io_t *io);
EVENT_EXPORT void e_io_ready(e_io_t *io);
EVENT_EXPORT int e_io_close(e_io_t *io);
EVENT_EXPORT int e_io_read(e_io_t *io);
EVENT_EXPORT int e_io_write(e_io_t *io, const void *buf, size_t len);
EVENT_EXPORT int    e_io_del(e_io_t* io, int events DEFAULT(EVENT_RDWR));
#define e_io_read_stop(io)  e_io_del(io, EVENT_READ)

void e_io_alloc_readbuf(e_io_t* io, int len);
bool e_io_is_alloced_readbuf(e_io_t* io);

// callback
EVENT_EXPORT void e_io_setcb_accept(e_io_t *io, e_accept_cb accept_cb);
EVENT_EXPORT void e_io_setcb_read(e_io_t *io, e_read_cb read_cb);
// callback - call
void e_io_accept_cb(e_io_t *io);
void e_io_connect_cb(e_io_t *io);
void e_io_write_cb(e_io_t *io, const void *buf, int len);
void e_io_handle_read(e_io_t* io, void* buf, int readbytes);
void e_io_read_cb(e_io_t* io, void* buf, int len);
// field - get
EVENT_EXPORT int e_io_fd(e_io_t *io);
EVENT_EXPORT struct sockaddr *e_io_localaddr(e_io_t *io);
EVENT_EXPORT struct sockaddr *e_io_peeraddr(e_io_t *io);
// field - set
EVENT_EXPORT void e_io_set_localaddr(e_io_t *io, struct sockaddr *addr, int addrlen);
EVENT_EXPORT void e_io_set_peeraddr(e_io_t *io, struct sockaddr *addr, int addrlen);

#endif