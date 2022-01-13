//
// Created by 20123460 on 2022/1/7.
//

#include "event/e_event.h"
#include "event/e_atomic.h"
#include "event/e_io.h"
#include "event/e_nio.h"
#include "event/e_loop.h"

static void fill_io_type(e_io_t *io) {
  int type = 0;
  socklen_t optlen = sizeof(int);
  int ret = getsockopt(io->fd, SOL_SOCKET, SO_TYPE, (char *) &type, &optlen);
  printd("getsockopt SO_TYPE fd=%d ret=%d type=%d errno=%d\n", io->fd, ret, type, socket_errno());
  if (ret == 0) {
    switch (type) {
      case SOCK_STREAM: io->io_type = EVENT_IO_TYPE_TCP;
        break;
      case SOCK_DGRAM: io->io_type = EVENT_IO_TYPE_UDP;
        break;
      case SOCK_RAW: io->io_type = EVENT_IO_TYPE_IP;
        break;
      default: io->io_type = EVENT_IO_TYPE_SOCKET;
        break;
    }
  } else if (socket_errno() == ENOTSOCK) {
    switch (io->fd) {
      case 0: io->io_type = EVENT_IO_TYPE_STDIN;
        break;
      case 1: io->io_type = EVENT_IO_TYPE_STDOUT;
        break;
      case 2: io->io_type = EVENT_IO_TYPE_STDERR;
        break;
      default: io->io_type = EVENT_IO_TYPE_FILE;
        break;
    }
  } else {
    io->io_type = EVENT_IO_TYPE_TCP;
  }
}

static void e_io_socket_init(e_io_t *io) {
  if ((io->io_type & EVENT_IO_TYPE_SOCK_DGRAM) || (io->io_type & EVENT_IO_TYPE_SOCK_RAW)) {
    // NOTE: sendto multiple peeraddr cannot use io->write_queue
    blocking(io->fd);
  } else {
    nonblocking(io->fd);
  }
  // fill io->localaddr io->peeraddr
  if (io->localaddr == NULL) {
    EVENT_ALLOC(io->localaddr, sizeof(sockaddr_u));
  }
  if (io->peeraddr == NULL) {
    EVENT_ALLOC(io->peeraddr, sizeof(sockaddr_u));
  }
  socklen_t addrlen = sizeof(sockaddr_u);
  int ret = getsockname(io->fd, io->localaddr, &addrlen);
  printd("getsockname fd=%d ret=%d errno=%d\n", io->fd, ret, socket_errno());
  // NOTE: udp peeraddr set by recvfrom/sendto
  if (io->io_type & EVENT_IO_TYPE_SOCK_STREAM) {
    addrlen = sizeof(sockaddr_u);
    ret = getpeername(io->fd, io->peeraddr, &addrlen);
    printd("getpeername fd=%d ret=%d errno=%d\n", io->fd, ret, socket_errno());
  }
}

void e_io_init(e_io_t *io) {
  e_recursive_mutex_init(&io->write_mutex);
}

void e_io_ready(e_io_t *io) {
  if (io->ready) return;
  // flags
  io->ready = 1;
  io->closed = 0;
  io->accept = io->connect = io->connectex = 0;
  io->recv = io->send = 0;
  io->recvfrom = io->sendto = 0;
  io->close = 0;
  // public:
  io->id = e_io_next_id();
  io->io_type = EVENT_IO_TYPE_UNKNOWN;
  io->error = 0;
  io->events = io->revents = 0;
  io->last_read_hrtime = io->last_write_hrtime = io->loop->cur_hrtime;
  // readbuf
  io->alloced_readbuf = 0;
  io->readbuf.base = io->loop->readbuf.base;
  io->readbuf.len = io->loop->readbuf.len;
  io->readbuf.head = io->readbuf.tail = 0;
  io->read_flags = 0;
  io->read_until_length = 0;
  io->small_readbytes_cnt = 0;
  // write_queue
  io->write_bufsize = 0;
  // callbacks
  io->read_cb = NULL;
  io->write_cb = NULL;
  io->close_cb = NULL;
  io->accept_cb = NULL;
  io->connect_cb = NULL;
  // timers
  io->connect_timeout = 0;
  io->connect_timer = NULL;
  io->close_timeout = 0;
  io->close_timer = NULL;
  io->read_timeout = 0;
  io->read_timer = NULL;
  io->write_timeout = 0;
  io->write_timer = NULL;
  io->keepalive_timeout = 0;
  io->keepalive_timer = NULL;
  io->heartbeat_interval = 0;
  io->heartbeat_fn = NULL;
  io->heartbeat_timer = NULL;
  // upstream
  io->upstream_io = NULL;
  // unpack
  io->unpack_setting = NULL;
  // ssl
  io->ssl = NULL;
  // context
  io->ctx = NULL;
  private:
#if defined(EVENT_POLL) || defined(EVENT_KQUEUE)
  io->event_index[0] = io->event_index[1] = -1;
#endif
#ifdef EVENT_IOCP
  io->hovlp = NULL;
#endif

  // io_type
  fill_io_type(io);
  if (io->io_type & EVENT_IO_TYPE_SOCKET) {
    e_io_socket_init(io);
  }

#if WITH_RUDP
  if ((io->io_type & HIO_TYPE_SOCK_DGRAM) || (io->io_type & HIO_TYPE_SOCK_RAW)) {
        rudp_init(&io->rudp);
    }
#endif
}

void e_io_done(e_io_t *io) {
  if (!io->ready) return;
  io->ready = 0;

  e_io_del(io, EVENT_RDWR);

  // readbuf
  e_io_free_readbuf(io);

  // write_queue
  offset_buf_t *pbuf = NULL;
  e_recursive_mutex_lock(&io->write_mutex);
  while (!write_queue_empty(&io->write_queue)) {
    pbuf = write_queue_front(&io->write_queue);
    EVENT_FREE(pbuf->base);
    write_queue_pop_front(&io->write_queue);
  }
  write_queue_cleanup(&io->write_queue);
  e_recursive_mutex_unlock(&io->write_mutex);

#if WITH_RUDP
  if ((io->io_type & HIO_TYPE_SOCK_DGRAM) || (io->io_type & HIO_TYPE_SOCK_RAW)) {
        rudp_cleanup(&io->rudp);
    }
#endif
}

void e_io_free(e_io_t *io) {
  if (io == NULL) return;
  e_io_close(io);
  e_recursive_mutex_destroy(&io->write_mutex);
  EVENT_FREE(io->localaddr);
  EVENT_FREE(io->peeraddr);
  EVENT_FREE(io);
}
uint64_t e_loop_next_event_id() {
  static e_atomic_t s_id = EVENT_ATOMIC_VAR_INIT(0);
  return ++s_id;
}

uint32_t e_io_next_id() {
  static e_atomic_t s_id = EVENT_ATOMIC_VAR_INIT(0);
  return ++s_id;
}