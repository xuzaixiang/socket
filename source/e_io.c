#include "event/e_io.h"

#include "e_socket.h"
#include "event/e_loop.h"

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
  io->readbuf.len = io->loop->readbuf.len;
  io->readbuf.base = io->loop->readbuf.base;
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

static void e_io_close_event_cb(e_event_t *ev) {
  e_io_t *io = (e_io_t *) ev->userdata;
  uint32_t id = (uintptr_t) ev->privdata;
  if (io->id != id)
    return;
  e_io_close(io);
}

int e_io_close_async(e_io_t *io) {
  e_event_t ev;
  memset(&ev, 0, sizeof(ev));
  ev.cb = e_io_close_event_cb;
  ev.userdata = io;
  ev.privdata = (void *) (uintptr_t) io->id;
  e_loop_post_event(io->loop, &ev);
  return 0;
}

int e_io_close(e_io_t *io) {

  return 0;
}

void e_io_free(e_io_t *io){

}