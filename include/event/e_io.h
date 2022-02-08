#ifndef EVENT_IO_H
#define EVENT_IO_H

#include "e_buf.h"
#include "e_sockaddr.h"
#include "e_unpack.h"

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
  // 1
  EVENT_IO_TYPE_STDIN = 0x00000001,
  EVENT_IO_TYPE_STDOUT = 0x00000002,
  EVENT_IO_TYPE_STDERR = 0x00000004,
  EVENT_IO_TYPE_STDIO = 0x0000000F,
  // 2
  EVENT_IO_TYPE_FILE = 0x00000010,
  // 3
  EVENT_IO_TYPE_IP = 0x00000100,
  EVENT_IO_TYPE_SOCK_RAW = 0x00000F00,
  // 45
  EVENT_IO_TYPE_UDP = 0x00001000,
  EVENT_IO_TYPE_KCP = 0x00002000,
  EVENT_IO_TYPE_DTLS = 0x00010000,
  EVENT_IO_TYPE_SOCK_DGRAM = 0x000FF000,
  // 67
  EVENT_IO_TYPE_TCP = 0x00100000,
  EVENT_IO_TYPE_SSL = 0x01000000,
  EVENT_IO_TYPE_TLS = EVENT_IO_TYPE_SSL,
  EVENT_IO_TYPE_SOCK_STREAM = 0x0FF00000,
  // 34567
  EVENT_IO_TYPE_SOCKET = 0x0FFFFF00,
} e_io_type_t;

typedef enum e_io_side_e {
  EVENT_IO_SERVER_SIDE = 0,
  EVENT_IO_CLIENT_SIDE = 1,
} e_io_side_t;

// e_io_read_flags
#define EVENT_IO_READ_ONCE 0x1
#define EVENT_IO_READ_UNTIL_LENGTH 0x2
#define EVENT_IO_READ_UNTIL_DELIM 0x4

struct e_io_s {
  EVENT_FIELDS
  unsigned ready : 1;
  unsigned closed : 1;
  unsigned accept : 1;
  unsigned connect : 1;
  unsigned close : 1;
  unsigned alloced_readbuf : 1; // for e_io_alloc_readbuf
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

  // read
  fifo_buf_t readbuf;
  unsigned int read_flags;
  // for e_io_read_until
  union {
    unsigned int read_until_length;
    unsigned char read_until_delim;
  };
  uint32_t small_readbytes_cnt; // for readbuf autosize

#if defined(EVENT_OS_MAC)
  int event_index[2]; // for poll,kqueue
#endif

  // unpack
  unpack_setting_t *unpack_setting; // for e_io_set_unpack

  // callback
  e_read_cb read_cb;
  e_accept_cb accept_cb;
  e_connect_cb connect_cb;
  e_write_cb write_cb;
};

// nio
// e_io_add(io, EVENT_READ) => accept => e_accept_cb
EVENT_EXPORT int e_io_accept(e_io_t *io);

EVENT_EXPORT void e_io_init(e_io_t *io);
EVENT_EXPORT void e_io_ready(e_io_t *io);
EVENT_EXPORT void e_io_free(e_io_t *io);

EVENT_EXPORT e_io_t *e_io_get(e_loop_t *loop, int fd);
EVENT_EXPORT int e_io_add(e_io_t *io, e_io_cb cb,
                          int events DEFAULT(EVENT_READ));
EVENT_EXPORT int e_io_del(e_io_t *io, int events DEFAULT(EVENT_RDWR));

EVENT_EXPORT int e_io_close(e_io_t *io);
EVENT_EXPORT int e_io_close_async(e_io_t *io);

EVENT_EXPORT int e_io_write(e_io_t *io, const void *buf, size_t len);

// read
EVENT_EXPORT int e_io_read(e_io_t *io);
#define e_io_read_start(io) e_io_read(io)
#define e_io_read_stop(io) e_io_del(io, EVENT_READ)
EVENT_EXPORT int
e_io_read_once(e_io_t *io); // e_io_read_start => e_read_cb => e_io_read_stop
EVENT_EXPORT int e_io_read_until_length(e_io_t *io, unsigned int len);
EVENT_EXPORT int e_io_read_until_delim(e_io_t *io, unsigned char delim);
EVENT_EXPORT int e_io_read_remain(e_io_t *io);
#define e_io_readline(io) e_io_read_until_delim(io, '\n')
#define e_io_readstring(io) e_io_read_until_delim(io, '\0')
#define e_io_readbytes(io, len) e_io_read_until_length(io, len)
#define e_io_read_until(io, len) e_io_read_until_length(io, len)

// unpack
EVENT_EXPORT void e_io_set_unpack(e_io_t* io, unpack_setting_t* setting);
EVENT_EXPORT void e_io_unset_unpack(e_io_t* io);

// callback - set
EVENT_EXPORT void e_io_setcb_accept(e_io_t *io, e_accept_cb accept_cb);
EVENT_EXPORT void e_io_setcb_read(e_io_t *io, e_read_cb read_cb);

// field - get
EVENT_EXPORT int e_io_fd(e_io_t *io);
EVENT_EXPORT struct sockaddr *e_io_localaddr(e_io_t *io);
EVENT_EXPORT struct sockaddr *e_io_peeraddr(e_io_t *io);

// field - set
EVENT_EXPORT void e_io_set_localaddr(e_io_t *io, struct sockaddr *addr,
                                     int addrlen);
EVENT_EXPORT void e_io_set_peeraddr(e_io_t *io, struct sockaddr *addr,
                                    int addrlen);

#endif