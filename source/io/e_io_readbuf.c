

#include "e_io_readbuf.h"
#include <event/e_io.h>
#include <event/e_loop.h>

void e_io_alloc_readbuf(e_io_t *io, int len) {
  if (len > EVENT_MAX_READ_BUFSIZE) {
    fprintf(stderr, "read bufsize > %u, close it!",
            (unsigned int)EVENT_MAX_READ_BUFSIZE);
    // todo
    e_io_close_async(io);
    return;
  }
  io->readbuf.base = (char *)e_realloc(io->readbuf.base, len);
  io->readbuf.len = len;
  io->small_readbytes_cnt = 0;
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

bool e_io_is_loop_readbuf(e_io_t *io) {
  return io->readbuf.base == io->loop->readbuf.base;
}

bool e_io_is_alloced_readbuf(e_io_t *io) { return io->alloced_readbuf; }
