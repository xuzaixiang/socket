#include "event/e_io.h"

#include "e_socket.h"
#include "event/e_loop.h"
#include "util/e_math.h"

void e_io_init(e_io_t *io) {
  // alloc localaddr,peeraddr when hio_socket_init
  /*
  if (io->localaddr == NULL) {
      HV_ALLOC(io->localaddr, sizeof(sockaddr_u));
  }
  if (io->peeraddr == NULL) {
      HV_ALLOC(io->peeraddr, sizeof(sockaddr_u));
  }
  */

  // write_queue init when hwrite try_write failed
  // write_queue_init(&io->write_queue, 4);

    e_recursive_mutex_init(&io->write_mutex);
}

void e_io_ready(e_io_t *io) {
  if (io->ready)
    return;
  // flags
  io->ready = 1;
  io->closed = 0;
  //  io->accept = io->connect = io->connectex = 0;
  //  io->recv = io->send = 0;
  //  io->recvfrom = io->sendto = 0;
  io->close = 0;
  // public:
  //  io->id = hio_next_id();
  //  io->io_type = HIO_TYPE_UNKNOWN;
  io->error = 0;
  io->events = io->revents = 0;
  //  io->last_read_hrtime = io->last_write_hrtime = io->loop->cur_hrtime;
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
  //  io->close_cb = NULL;
  io->accept_cb = NULL;
  io->connect_cb = NULL;
  // timers
//  io->connect_timeout = 0;
//  io->connect_timer = NULL;
//  io->close_timeout = 0;
//  io->close_timer = NULL;
//  io->read_timeout = 0;
//  io->read_timer = NULL;
//  io->write_timeout = 0;
//  io->write_timer = NULL;
//  io->keepalive_timeout = 0;
//  io->keepalive_timer = NULL;
//  io->heartbeat_interval = 0;
//  io->heartbeat_fn = NULL;
//  io->heartbeat_timer = NULL;
//   upstream
//  io->upstream_io = NULL;
//   unpack
//  io->unpack_setting = NULL;
//   ssl
//  io->ssl = NULL;
//  io->ssl_ctx = NULL;
//  io->alloced_ssl_ctx = 0;
//   context
//  io->ctx = NULL;
#if defined(EVENT_OS_MAC)
  io->event_index[0] = io->event_index[1] = -1;
#endif

  e_socket_init(io);
}

e_io_t *e_io_get(e_loop_t *loop, int fd) {
  if (fd >= loop->ios.maxsize) {
    int newsize = ceil2e(fd);
    io_array_resize(&loop->ios, newsize > fd ? newsize : 2 * fd);
  }
  e_io_t *io = loop->ios.ptr[fd];
  if (io == NULL) {
    EVENT_ALLOC_SIZEOF(io);
    e_io_init(io);
    io->event_type = EVENT_TYPE_IO;
    io->loop = loop;
    io->fd = fd;
    loop->ios.ptr[fd] = io;
  }
  if (!io->ready) {
    e_io_ready(io);
  }

  return io;
}

int e_io_add(e_io_t *io, e_io_cb cb, int events) {
//  printd("hio_add fd=%d io->events=%d events=%d\n", io->fd, io->events,
//  events);
#ifdef EVENT_OS_WIN
  // Windows iowatcher not work on stdio
  if (io->fd < 3)
    return -1;
#endif
  e_loop_t *loop = io->loop;
  if (!io->active) {
    EVENT_ADD(loop, io, cb);
    loop->nios++;
  }
  if (!io->ready) {
    e_io_ready(io);
  }
  if (cb) {
    io->cb = (e_event_cb)cb;
  }
  if (!(io->events & events)) {
    e_iowatcher_add_event(loop, io->fd, events);
    io->events |= events;
  }
  return 0;
}

void e_io_handle_read(e_io_t *io, void *buf, int readbytes) {
#if WITH_KCP
  if (io->io_type == HIO_TYPE_KCP) {
    hio_read_kcp(io, buf, readbytes);
    return;
  }
#endif

  //  if (io->unpack_setting) {
  //     hio_set_unpack
  //    hio_unpack(io, buf, readbytes);
  //  } else {
  const unsigned char *sp =
      (const unsigned char *)io->readbuf.base + io->readbuf.head;
  const unsigned char *ep = (const unsigned char *)buf + readbytes;
  if (io->read_flags & EVENT_IO_READ_UNTIL_LENGTH) {
    // hio_read_until_length
    if (ep - sp >= io->read_until_length) {
      io->readbuf.head += io->read_until_length;
      if (io->readbuf.head == io->readbuf.tail) {
        io->readbuf.head = io->readbuf.tail = 0;
      }
      io->read_flags &= ~EVENT_IO_READ_UNTIL_LENGTH;
      e_io_read_cb(io, (void *)sp, io->read_until_length);
    }
  } else if (io->read_flags & EVENT_IO_READ_UNTIL_DELIM) {
    // hio_read_until_delim
    const unsigned char *p = (const unsigned char *)buf;
    for (int i = 0; i < readbytes; ++i, ++p) {
      if (*p == io->read_until_delim) {
        int len = p - sp + 1;
        io->readbuf.head += len;
        if (io->readbuf.head == io->readbuf.tail) {
          io->readbuf.head = io->readbuf.tail = 0;
        }
        io->read_flags &= ~EVENT_IO_READ_UNTIL_DELIM;
        e_io_read_cb(io, (void *)sp, len);
        return;
      }
    }
  } else {
    // hio_read
    io->readbuf.head = io->readbuf.tail = 0;
    e_io_read_cb(io, (void *)sp, ep - sp);
  }
  //  }

  if (io->readbuf.head == io->readbuf.tail) {
    io->readbuf.head = io->readbuf.tail = 0;
  }
  // readbuf autosize
  if (io->readbuf.tail == io->readbuf.len) {
    if (io->readbuf.head == 0) {
      // scale up * 2
      e_io_alloc_readbuf(io, io->readbuf.len * 2);
    } else {
      // [head, tail] => [base, tail - head]
      memmove(io->readbuf.base, io->readbuf.base + io->readbuf.head,
              io->readbuf.tail - io->readbuf.head);
    }
  } else {
    size_t small_size = io->readbuf.len / 2;
    if (io->readbuf.tail < small_size && io->small_readbytes_cnt >= 3) {
      // scale down / 2
      e_io_alloc_readbuf(io, small_size);
    }
  }
}

bool e_io_is_alloced_readbuf(e_io_t *io) { return io->alloced_readbuf; }

void e_io_alloc_readbuf(e_io_t *io, int len) {
  if (len > EVENT_MAX_READ_BUFSIZE) {
    fprintf(stderr, "read bufsize > %u, close it!",
            (unsigned int)EVENT_MAX_READ_BUFSIZE);
    //    e_io_close_async(io);
    return;
  }
  if (e_io_is_alloced_readbuf(io)) {
    io->readbuf.base = (char *)e_realloc(io->readbuf.base, len);
  } else {
    EVENT_ALLOC(io->readbuf.base, len);
  }
  io->readbuf.len = len;
  io->alloced_readbuf = 1;
  io->small_readbytes_cnt = 0;
}

static void e_io_close_event_cb(e_event_t *ev) {
  e_io_t *io = (e_io_t *)ev->userdata;
  uint32_t id = (uintptr_t)ev->privdata;
  if (io->id != id)
    return;
  e_io_close(io);
}

int e_io_close_async(e_io_t *io) {
  e_event_t ev;
  memset(&ev, 0, sizeof(ev));
  ev.cb = e_io_close_event_cb;
  ev.userdata = io;
  ev.privdata = (void *)(uintptr_t)io->id;
  e_loop_post_event(io->loop, &ev);
  return 0;
}