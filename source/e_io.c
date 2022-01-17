#include "event/e_io.h"

#include "event/e_loop.h"
#include "util/e_math.h"

void e_io_init(e_io_t* io){
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

e_io_t *e_io_get(e_loop_t *loop, int fd) {
  if (fd >= loop->ios.maxsize) {
    int newsize = ceil2e(fd);
    io_array_resize(&loop->ios, newsize > fd ? newsize : 2*fd);
  }
  e_io_t* io = loop->ios.ptr[fd];
  if (io == NULL) {
    EVENT_ALLOC_SIZEOF(io);
    e_io_init(io);
    io->event_type = EVENT_TYPE_IO;
    io->loop = loop;
    io->fd = fd;
    loop->ios.ptr[fd] = io;
  }
}