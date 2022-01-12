#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

#include "event/e_io.h"
#include "event/e_socket.h"
#include "event/e_event.h"
#include "event/e_api.h"
#include "event/e_loop.h"

#include "../util/e_math.h"
#include "../e_watcher.h"

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
//  io->id = hio_next_id();
//  io->io_type = HIO_TYPE_UNKNOWN;
  io->error = 0;
  io->events = io->revents = 0;
  io->last_read_hrtime = io->last_write_hrtime = io->loop->cur_hrtime;
  // readbuf
  io->alloced_readbuf = 0;
//  io->readbuf.base = io->loop->readbuf.base;
//  io->readbuf.len = io->loop->readbuf.len;
//  io->readbuf.head = io->readbuf.tail = 0;
//  io->read_flags = 0;
//  io->read_until_length = 0;
//  io->small_readbytes_cnt = 0;
//  // write_queue
//  io->write_bufsize = 0;
//  // callbacks
//  io->read_cb = NULL;
//  io->write_cb = NULL;
//  io->close_cb = NULL;
//  io->accept_cb = NULL;
//  io->connect_cb = NULL;
//  // timers
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
//  // upstream
//  io->upstream_io = NULL;
//  // unpack
//  io->unpack_setting = NULL;
//  // ssl
//  io->ssl = NULL;
//  // context
//  io->ctx = NULL;
  // private:
#if defined(EVENT_POLL) || defined(EVENT_KQUEUE)
  io->event_index[0] = io->event_index[1] = -1;
#endif
#ifdef EVENT_IOCP
  io->hovlp = NULL;
#endif

  // io_type
//  fill_io_type(io);
//  if (io->io_type & HIO_TYPE_SOCKET) {
//    hio_socket_init(io);
//  }

#if WITH_RUDP
  if ((io->io_type & HIO_TYPE_SOCK_DGRAM) || (io->io_type & HIO_TYPE_SOCK_RAW)) {
        rudp_init(&io->rudp);
    }
#endif
}
void e_io_accept_cb(e_io_t *io) {
  /*
   char localaddrstr[SOCKADDR_STRLEN] = {0};
   char peeraddrstr[SOCKADDR_STRLEN] = {0};
   printd("accept connfd=%d [%s] <= [%s]\n", io->fd,
           SOCKADDR_STR(io->localaddr, localaddrstr),
           SOCKADDR_STR(io->peeraddr, peeraddrstr));
   */
  if (io->accept_cb) {
    // printd("accept_cb------\n");
    io->accept_cb(io);
    // printd("accept_cb======\n");
  }
}
static void __accept_cb(e_io_t *io) {
  e_io_accept_cb(io);
}

static void nio_accept(e_io_t *io) {
  // printd("nio_accept listenfd=%d\n", io->fd);
  int connfd = 0, err = 0, accept_cnt = 0;
  socklen_t addrlen;
  e_io_t *connio = NULL;
  while (accept_cnt++ < 3) {
    addrlen = sizeof(sockaddr_u);
    connfd = accept(io->fd, io->peeraddr, &addrlen);
    if (connfd < 0) {
      err = socket_errno();
      if (err == EAGAIN || err == EINTR) {
        return;
      } else {
        perror("accept");
        io->error = err;
        goto accept_error;
      }
    }
    addrlen = sizeof(sockaddr_u);
    getsockname(connfd, io->localaddr, &addrlen);
    connio = e_io_get(io->loop, connfd);
    // NOTE: inherit from listenio
    connio->accept_cb = io->accept_cb;
    connio->userdata = io->userdata;
//    if (io->unpack_setting) {
//      hio_set_unpack(connio, io->unpack_setting);
//    }

    if (io->io_type == EVENT_IO_TYPE_SSL) {
//      if (connio->ssl == NULL) {
//        hssl_ctx_t ssl_ctx = hssl_ctx_instance();
//        if (ssl_ctx == NULL) {
//          io->error = HSSL_ERROR;
//          goto accept_error;
//        }
//        hssl_t ssl = hssl_new(ssl_ctx, connfd);
//        if (ssl == NULL) {
//          io->error = HSSL_ERROR;
//          goto accept_error;
//        }
//        connio->ssl = ssl;
//      }
//      hio_enable_ssl(connio);
//      ssl_server_handshake(connio);
    } else {
//      // NOTE: SSL call accept_cb after handshake finished
      __accept_cb(connio);
    }
  }
  return;

  accept_error:
//  hloge("listenfd=%d accept error: %s:%d", io->fd, socket_strerror(io->error), io->error);
  e_io_close(io);
}
static void e_io_close_event_cb(e_event_t *ev) {
  e_io_t *io = (e_io_t *) ev->userdata;
  uint32_t id = (uintptr_t) ev->privdata;
  if (io->id != id) return;
  e_io_close(io);
}
void e_io_free_readbuf(e_io_t *io) {
  if (e_io_is_alloced_readbuf(io)) {
    EVENT_FREE(io->readbuf.base);
    io->alloced_readbuf = 0;
    // reset to loop->readbuf
    io->readbuf.base = io->loop->readbuf.base;
    io->readbuf.len = io->loop->readbuf.len;
  }
}
int hio_close_async(e_io_t *io) {
  e_event_t ev;
  memset(&ev, 0, sizeof(ev));
  ev.cb = e_io_close_event_cb;
  ev.userdata = io;
  ev.privdata = (void *) (uintptr_t) io->id;
  e_loop_post_event(io->loop, &ev);
  return 0;
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
void e_io_close_cb(e_io_t *io) {
  if (io->close_cb) {
    // printd("close_cb------\n");
    io->close_cb(io);
    // printd("close_cb======\n");
  }
}
static void __close_cb(e_io_t *io) {
  // printd("close fd=%d\n", io->fd);
//  e_io_del_connect_timer(io);
//  e_io_del_close_timer(io);
//  e_io_del_read_timer(io);
//  e_io_del_write_timer(io);
//  e_io_del_keepalive_timer(io);
//  e_io_del_heartbeat_timer(io);
  e_io_close_cb(io);
}

int e_io_close(e_io_t *io) {
  if (io->closed) return 0;
  if (e_gettid() != io->loop->tid) {
    return hio_close_async(io);
  }

  e_recursive_mutex_lock(&io->write_mutex);
  if (io->closed) {
    e_recursive_mutex_unlock(&io->write_mutex);
    return 0;
  }
  if (!write_queue_empty(&io->write_queue) && io->error == 0 && io->close == 0) {
    io->close = 1;
    e_recursive_mutex_unlock(&io->write_mutex);
//    hlogw("write_queue not empty, close later.");
//    int timeout_ms = io->close_timeout ? io->close_timeout : HIO_DEFAULT_CLOSE_TIMEOUT;
//    io->close_timer = htimer_add(io->loop, __close_timeout_cb, timeout_ms, 1);
//    io->close_timer->privdata = io;
    return 0;
  }
  io->closed = 1;
  e_recursive_mutex_unlock(&io->write_mutex);

  e_io_done(io);
  __close_cb(io);
  if (io->ssl) {
//    e_ssl_free(io->ssl);
    io->ssl = NULL;
  }
  if (io->io_type & EVENT_IO_TYPE_SOCKET) {
    closesocket(io->fd);
  }
  return 0;
}
void e_io_init(e_io_t *io) {
  e_recursive_mutex_init(&io->write_mutex);
}

int hio_add(e_io_t *io, e_io_cb cb, int events DEFAULT(EVENT_READ)) {
  printd("hio_add fd=%d io->events=%d events=%d\n", io->fd, io->events, events);
#ifdef OS_WIN
  // Windows iowatcher not work on stdio
    if (io->fd < 3) return -1;
#endif
  e_loop_t *loop = io->loop;
//  if (!io->active) {
//    EVENT_ADD(loop, io, cb);
//    loop->nios++;
//  }
//
//  if (!io->ready) {
//    e_io_ready(io);
//  }
//
//  if (cb) {
//    io->cb = (e_event_cb)cb;
//  }
//
//  if (!(io->events & events)) {
//    iowatcher_add_event(loop, io->fd, events);
//    io->events |= events;
//  }
  return 0;
}
int e_io_fd(e_io_t *io) {
  return io->fd;
}
int e_io_error(e_io_t *io) {
  return io->error;
}
void e_io_free(e_io_t *io) {
  if (io == NULL) return;
  e_io_close(io);
  e_recursive_mutex_destroy(&io->write_mutex);
  EVENT_FREE(io->localaddr);
  EVENT_FREE(io->peeraddr);
  EVENT_FREE(io);
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

void e_io_set_localaddr(e_io_t *io, struct sockaddr *addr, int addrlen) {
  if (io->localaddr == NULL) {
    EVENT_ALLOC(io->localaddr, sizeof(sockaddr_u));
  }
  memcpy(io->localaddr, addr, addrlen);
}
void e_io_set_peeraddr(e_io_t *io, struct sockaddr *addr, int addrlen) {
  if (io->peeraddr == NULL) {
    EVENT_ALLOC(io->peeraddr, sizeof(sockaddr_u));
  }
  memcpy(io->peeraddr, addr, addrlen);
}
e_io_t *e_io_create_socket(e_loop_t *loop, const char *host, int port,
                           e_io_type_e type DEFAULT(EVENT_IO_TYPE_TCP),
                           e_io_side_e side DEFAULT(EVENT_IO_SERVER_SIDE)) {
  int sock_type = type & EVENT_IO_TYPE_SOCK_STREAM ? SOCK_STREAM :
                  type & EVENT_IO_TYPE_SOCK_DGRAM ? SOCK_DGRAM :
                  type & EVENT_IO_TYPE_SOCK_RAW ? SOCK_RAW : -1;
  if (sock_type == -1) return NULL;
  sockaddr_u addr;
  memset(&addr, 0, sizeof(addr));
  int ret = -1;
#ifdef ENABLE_UDS
  if (port < 0) {
        sockaddr_set_path(&addr, host);
        ret = 0;
    }
#endif
  if (port >= 0) {
    ret = sockaddr_set_ipport(&addr, host, port);
  }
  if (ret != 0) {
    fprintf(stderr, "unknown host: %s\n", host);
    return NULL;
  }
  int sockfd = socket(addr.sa.sa_family, sock_type, 0);
  if (sockfd < 0) {
    perror("socket");
    return NULL;
  }
  e_io_t *io = NULL;
  if (side == EVENT_IO_SERVER_SIDE) {
#ifdef SO_REUSEADDR
    // NOTE: SO_REUSEADDR allow to reuse sockaddr of TIME_WAIT status
    int reuseaddr = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *) &reuseaddr, sizeof(int)) < 0) {
      perror("setsockopt");
      closesocket(sockfd);
      return NULL;
    }
#endif
    if (bind(sockfd, &addr.sa, sockaddr_len(&addr)) < 0) {
      perror("bind");
      closesocket(sockfd);
      return NULL;
    }
    if (sock_type == SOCK_STREAM) {
      if (listen(sockfd, SOMAXCONN) < 0) {
        perror("listen");
        closesocket(sockfd);
        return NULL;
      }
    }
  }
  io = e_io_get(loop, sockfd);
  assert(io != NULL);
  io->io_type = type;
  if (side == EVENT_IO_SERVER_SIDE) {
    e_io_set_localaddr(io, &addr.sa, sockaddr_len(&addr));
    io->priority = EVENT_HIGH_PRIORITY;
  } else {
    e_io_set_peeraddr(io, &addr.sa, sockaddr_len(&addr));
  }
  return io;
}

void e_io_setcb_accept(e_io_t *io, e_accept_cb accept_cb) {
  io->accept_cb = accept_cb;
}
void e_io_setcb_close(e_io_t *io, e_close_cb close_cb) {
  io->close_cb = close_cb;
}
void e_io_setcb_connect(e_io_t *io, e_connect_cb connect_cb) {
  io->connect_cb = connect_cb;
}
void e_io_setcb_read(e_io_t *io, e_read_cb read_cb) {
  io->read_cb = read_cb;
}
void e_io_setcb_write(e_io_t *io, e_write_cb write_cb) {
  io->write_cb = write_cb;
}

static void e_io_handle_events(e_io_t *io) {
  if ((io->events & EVENT_READ) && (io->revents & EVENT_READ)) {
    if (io->accept) {
      nio_accept(io);
    } else {
//      nio_read(io);
    }
  }

//  if ((io->events & EVENT_WRITE) && (io->revents & EVENT_WRITE)) {
//    // NOTE: del HV_WRITE, if write_queue empty
//    hrecursive_mutex_lock(&io->write_mutex);
//    if (write_queue_empty(&io->write_queue)) {
//      iowatcher_del_event(io->loop, io->fd, HV_WRITE);
//      io->events &= ~HV_WRITE;
//    }
//    hrecursive_mutex_unlock(&io->write_mutex);
//    if (io->connect) {
//      // NOTE: connect just do once
//      // ONESHOT
//      io->connect = 0;
//
//      nio_connect(io);
//    }
//    else {
//      nio_write(io);
//    }
//  }

  io->revents = 0;
}

int e_io_accept(e_io_t *io) {
  io->accept = 1;
  e_io_add(io, e_io_handle_events, EVENT_READ);
  return 0;
}

int e_io_add(e_io_t *io, e_io_cb cb, int events DEFAULT(EVENT_READ)) {
  printd("hio_add fd=%d io->events=%d events=%d\n", io->fd, io->events, events);
#ifdef OS_WIN
  // Windows iowatcher not work on stdio
    if (io->fd < 3) return -1;
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
    io->cb = (e_event_cb) cb;
  }

  if (!(io->events & events)) {
    iowatcher_add_event(loop, io->fd, events);
    io->events |= events;
  }
  return 0;
}
int e_io_read(e_io_t *io) {
  if (io->closed) {
//    hloge("hio_read called but fd[%d] already closed!", io->fd);
    return -1;
  }
  hio_add(io, e_io_handle_events, EVENT_READ);
  if (io->readbuf.tail > io->readbuf.head &&
      io->unpack_setting == NULL &&
      io->read_flags == 0) {
    e_io_read_remain(io);
  }
  return 0;
}
int e_io_read_remain(e_io_t *io) {
  int remain = io->readbuf.tail - io->readbuf.head;
  if (remain > 0) {
    void *buf = io->readbuf.base + io->readbuf.head;
    io->readbuf.head = io->readbuf.tail = 0;
    e_io_read_cb(io, buf, remain);
  }
  return remain;
}
int e_io_del(e_io_t *io, int events DEFAULT(EVENT_RDWR)) {
  printd("hio_del fd=%d io->events=%d events=%d\n", io->fd, io->events, events);
#ifdef OS_WIN
  // Windows iowatcher not work on stdio
    if (io->fd < 3) return -1;
#endif
  if (!io->active) return -1;

  if (io->events & events) {
    iowatcher_del_event(io->loop, io->fd, events);
    io->events &= ~events;
  }
  if (io->events == 0) {
    io->loop->nios--;
    // NOTE: not EVENT_DEL, avoid free
    EVENT_INACTIVE(io);
  }
  return 0;
}
bool e_io_is_alloced_readbuf(e_io_t *io) {
  return io->alloced_readbuf;
}
void e_io_read_cb(e_io_t *io, void *buf, int len) {
  if (io->read_flags & EVENT_IO_READ_ONCE) {
    io->read_flags &= ~EVENT_IO_READ_ONCE;
    e_io_read_stop(io);
  }

  if (io->read_cb) {
    // printd("read_cb------\n");
    io->read_cb(io, buf, len);
    // printd("read_cb======\n");
  }

  // for readbuf autosize
  if (e_io_is_alloced_readbuf(io) && io->readbuf.len > EVENT_READ_BUFSIZE_HIGH_WATER) {
    size_t small_size = io->readbuf.len / 2;
    if (len < small_size) {
      ++io->small_readbytes_cnt;
    } else {
      io->small_readbytes_cnt = 0;
    }
  }
}

static int __nio_write(e_io_t *io, const void *buf, int len) {
  int nwrite = 0;
  switch (io->io_type) {
    case EVENT_IO_TYPE_SSL:
//      nwrite = e_ssl_write(io->ssl, buf, len);
      break;
    case EVENT_IO_TYPE_TCP:
#ifdef EVENT_OS_LINUX
      nwrite = write(io->fd, buf, len);
#else
      nwrite = send(io->fd, buf, len, 0);
#endif
      break;
    case EVENT_IO_TYPE_UDP:
    case EVENT_IO_TYPE_KCP:
    case EVENT_IO_TYPE_IP:
//      nwrite = sendto(io->fd, buf, len, 0, io->peeraddr, SOCKADDR_LEN(io->peeraddr));
      break;
    default:nwrite = write(io->fd, buf, len);
      break;
  }
  // hlogd("write retval=%d", nwrite);
  return nwrite;
}
void e_io_write_cb(e_io_t *io, const void *buf, int len) {
  if (io->write_cb) {
    // printd("write_cb------\n");
    io->write_cb(io, buf, len);
    // printd("write_cb======\n");
  }
}
static void __write_cb(e_io_t *io, const void *buf, int writebytes) {
  // printd("< %.*s\n", writebytes, buf);
  io->last_write_hrtime = io->loop->cur_hrtime;
  e_io_write_cb(io, buf, writebytes);
}

int e_io_write(e_io_t *io, const void *buf, size_t len) {
  if (io->closed) {
//    hloge("hio_write called but fd[%d] already closed!", io->fd);
    return -1;
  }
  int nwrite = 0, err = 0;
  e_recursive_mutex_lock(&io->write_mutex);
#if WITH_KCP
  if (io->io_type == HIO_TYPE_KCP) {
        nwrite = hio_write_kcp(io, buf, len);
        if (nwrite < 0) goto write_error;
        goto write_done;
    }
#endif
  if (write_queue_empty(&io->write_queue)) {
    try_write:
    nwrite = __nio_write(io, buf, len);
    // printd("write retval=%d\n", nwrite);
    if (nwrite < 0) {
      err = socket_errno();
      if (err == EAGAIN) {
        nwrite = 0;
//        hlogw("try_write failed, enqueue!");
        goto enqueue;
      } else {
        // perror("write");
        io->error = err;
        goto write_error;
      }
    }
    if (nwrite == 0) {
      goto disconnect;
    }
    if (nwrite == len) {
      goto write_done;
    }
    enqueue:
    hio_add(io, e_io_handle_events, EVENT_WRITE);
  }
  if (nwrite < len) {
    if (io->write_bufsize + len - nwrite > EVENT_MAX_WRITE_BUFSIZE) {
      if (io->write_bufsize > EVENT_MAX_WRITE_BUFSIZE) {
//        hloge("write bufsize > %u, close it!", (unsigned int)MAX_WRITE_BUFSIZE);
        goto write_error;
      }
    }
    offset_buf_t remain;
    remain.len = len - nwrite;
    remain.offset = 0;
    // NOTE: free in nio_write
    EVENT_ALLOC(remain.base, remain.len);
    memcpy(remain.base, ((char *) buf) + nwrite, remain.len);
    if (io->write_queue.maxsize == 0) {
      write_queue_init(&io->write_queue, 4);
    }
    write_queue_push_back(&io->write_queue, &remain);
    io->write_bufsize += remain.len;
    if (io->write_bufsize > EVENT_WRITE_BUFSIZE_HIGH_WATER) {
//      hlogw("write len=%d enqueue %u, bufsize=%u over high water %u",
//            len, (unsigned int)(remain.len - remain.offset),
//            (unsigned int)io->write_bufsize,
//            (unsigned int)WRITE_BUFSIZE_HIGH_WATER);
    }
  }
  write_done:
  e_recursive_mutex_unlock(&io->write_mutex);
  if (nwrite > 0) {
    __write_cb(io, buf, nwrite);
  }
  return nwrite;
  write_error:
  disconnect:
  e_recursive_mutex_unlock(&io->write_mutex);
  /* NOTE:
   * We usually free resources in hclose_cb,
   * if hio_close_sync, we have to be very careful to avoid using freed resources.
   * But if hio_close_async, we do not have to worry about this.
   */
  hio_close_async(io);
  return nwrite < 0 ? nwrite : -1;
}
struct sockaddr *e_io_localaddr(e_io_t *io) {
  return io->localaddr;
}

struct sockaddr *e_io_peeraddr(e_io_t *io) {
  return io->peeraddr;
}
