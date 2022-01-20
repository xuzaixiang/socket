#include "event/e_io.h"

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

//  hrecursive_mutex_init(&io->write_mutex);
}
void e_io_ready(e_io_t *io) {
  if (io->ready) return;
  // flags
  io->ready = 1;
  io->closed = 0;
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
//  printd("hio_add fd=%d io->events=%d events=%d\n", io->fd, io->events, events);
#ifdef EVENT_OS_WIN
  // Windows iowatcher not work on stdio
    if (io->fd < 3) return -1;
#endif
  e_loop_t* loop = io->loop;
  if (!io->active) {
    EVENT_ADD(loop, io, cb);
//    loop->nios++;
  }
  if (!io->ready) {
    e_io_ready(io);
  }
  if (cb) {
    io->cb = (e_event_cb)cb;
  }
  if (!(io->events & events)) {
    iowatcher_add_event(loop, io->fd, events);
    io->events |= events;
  }
  return 0;
}
void e_io_set_localaddr(e_io_t *io, struct sockaddr *addr, int addrlen) {
  if (io->localaddr == NULL) {
    EVENT_ALLOC(io->localaddr, sizeof(e_sockaddr_t));
  }
  memcpy(io->localaddr, addr, addrlen);
}
void e_io_set_peeraddr(e_io_t *io, struct sockaddr *addr, int addrlen) {
  if (io->peeraddr == NULL) {
    EVENT_ALLOC(io->peeraddr, sizeof(e_sockaddr_t));
  }
  memcpy(io->peeraddr, addr, addrlen);
}
