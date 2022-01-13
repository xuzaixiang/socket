#include "event/e_io.h"
#include "event/e_loop.h"

bool e_io_is_loop_readbuf(e_io_t* io) {
  return io->readbuf.base == io->loop->readbuf.base;
}

bool e_io_is_alloced_readbuf(e_io_t *io) {
  return io->alloced_readbuf;
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

void e_io_alloc_readbuf(e_io_t* io, int len){
  if (len > EVENT_MAX_READ_BUFSIZE) {
//    hloge("read bufsize > %u, close it!", (unsigned int)MAX_READ_BUFSIZE);
    hio_close_async(io);
    return;
  }
  if (e_io_is_alloced_readbuf(io)) {
    io->readbuf.base = (char*)e_realloc_safe(io->readbuf.base, len, io->readbuf.len);
  } else {
    EVENT_ALLOC(io->readbuf.base, len);
  }
  io->readbuf.len = len;
  io->alloced_readbuf = 1;
  io->small_readbytes_cnt = 0;
}
