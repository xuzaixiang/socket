#include "event/e_io.h"
#include "event/e_loop.h"

uint32_t e_io_id(e_io_t *io) {
  return io->id;
}
int e_io_fd(e_io_t *io) {
  return io->fd;
}
int e_io_error(e_io_t *io) {
  return io->error;
}
int e_io_events(e_io_t *io) {
  return io->events;
}
int e_io_revents(e_io_t *io) {
  return io->revents;
}
e_io_type_e e_io_type(e_io_t *io) {
  return io->io_type;
}

struct sockaddr *e_io_localaddr(e_io_t *io) {
  return io->localaddr;
}

struct sockaddr *e_io_peeraddr(e_io_t *io) {
  return io->peeraddr;
}
void e_io_set_context(e_io_t *io, void *ctx) {
  io->ctx = ctx;
}
void *e_io_context(e_io_t *io) {
  return io->ctx;
}
bool e_io_is_opened(e_io_t *io) {
  if (io == NULL) return false;
  return io->ready == 1 && io->closed == 0;
}
bool e_io_is_closed(e_io_t *io) {
  if (io == NULL) return true;
  return io->ready == 0 && io->closed == 1;
}
fifo_buf_t *e_io_get_readbuf(e_io_t *io) {
  return &io->readbuf;
}
size_t e_io_write_bufsize(e_io_t *io) {
  return io->write_bufsize;
}
bool e_io_write_queue_is_empty(e_io_t *io) {
  return io->write_bufsize == 0;
}
uint64_t e_io_last_read_time(e_io_t *io) {
  e_loop_t *loop = io->loop;
  return loop->start_ms + (io->last_read_hrtime - loop->start_hrtime) / 1000;
}
uint64_t e_io_last_write_time(e_io_t *io) {
  e_loop_t* loop = io->loop;
  return loop->start_ms + (io->last_write_hrtime - loop->start_hrtime) / 1000;
}