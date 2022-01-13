#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

#include "event/e_io.h"
#include "event/e_nio.h"
#include "event/e_socket.h"
#include "event/e_event.h"
#include "event/e_api.h"
#include "event/e_loop.h"

#include "../util/e_math.h"
#include "../e_watcher.h"


static void e_io_close_event_cb(e_event_t *ev) {
  e_io_t *io = (e_io_t *) ev->userdata;
  uint32_t id = (uintptr_t) ev->privdata;
  if (io->id != id) return;
  e_io_close(io);
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
