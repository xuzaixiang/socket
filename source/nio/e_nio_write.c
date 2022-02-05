//
// Created by 20123460 on 2022/2/2.
//
#include "e_nio.h"

static void __write_cb(e_io_t *io, const void *buf, int writebytes) {
  // printd("< %.*s\n", writebytes, buf);
  //  io->last_write_hrtime = io->loop->cur_hrtime;
  e_io_write_cb(io, buf, writebytes);
}

static int __nio_write(e_io_t *io, const void *buf, int len) {
  int nwrite = 0;
  switch (io->io_type) {
  case EVENT_IO_TYPE_SSL:
    //      nwrite = hssl_write(io->ssl, buf, len);
    break;
  case EVENT_IO_TYPE_TCP:
#ifdef EVENT_OS_UNIX
    nwrite = write(io->fd, buf, len);
#else
    nwrite = send(io->fd, buf, len, 0);
#endif
    break;
  case EVENT_IO_TYPE_UDP:
  case EVENT_IO_TYPE_KCP:
  case EVENT_IO_TYPE_IP:
    nwrite = sendto(io->fd, buf, len, 0, io->peeraddr,
                    EVENT_SOCKADDR_LEN(io->peeraddr));
    break;
  default:
    nwrite = write(io->fd, buf, len);
    break;
  }
  // hlogd("write retval=%d", nwrite);
  return nwrite;
}

void e_nio_write(e_io_t *io) {
  // printd("nio_write fd=%d\n", io->fd);
  int nwrite = 0, err = 0;
  e_recursive_mutex_lock(&io->write_mutex);
write:
  if (write_queue_empty(&io->write_queue)) {
    e_recursive_mutex_unlock(&io->write_mutex);
    if (io->close) {
      io->close = 0;
      e_io_close(io);
    }
    return;
  }
  offset_buf_t *pbuf = write_queue_front(&io->write_queue);
  char *base = pbuf->base;
  char *buf = base + pbuf->offset;
  int len = pbuf->len - pbuf->offset;
  nwrite = __nio_write(io, buf, len);
  // printd("write retval=%d\n", nwrite);
  if (nwrite < 0) {
    err = e_socket_errno();
    if (err == EAGAIN) {
      e_recursive_mutex_unlock(&io->write_mutex);
      return;
    } else {
      // perror("write");
      io->error = err;
      goto write_error;
    }
  }
  if (nwrite == 0) {
    goto disconnect;
  }
  pbuf->offset += nwrite;
  io->write_bufsize -= nwrite;
  __write_cb(io, buf, nwrite);
  if (nwrite == len) {
    // NOTE: after write_cb, pbuf maybe invalid.
    // HV_FREE(pbuf->base);
    EVENT_FREE(base);
    write_queue_pop_front(&io->write_queue);
    if (!io->closed) {
      // write continue
      goto write;
    }
  }
  e_recursive_mutex_unlock(&io->write_mutex);
  return;
write_error:
disconnect:
  e_recursive_mutex_unlock(&io->write_mutex);
  e_io_close(io);
}

int e_io_write(e_io_t *io, const void *buf, size_t len) {
  if (io->closed) {
    fprintf(stderr, "e_io_write called but fd[%d] already closed!", io->fd);
    return -1;
  }
  int nwrite = 0, err = 0;
  e_recursive_mutex_lock(&io->write_mutex);
  if (write_queue_empty(&io->write_queue)) {
  try_write:
    nwrite = __nio_write(io, buf, len);
    // printd("write retval=%d\n", nwrite);
    if (nwrite < 0) {
      err = e_socket_errno();
      if (err == EAGAIN) {
        nwrite = 0;
        perror("try_write failed, enqueue!");
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
    e_io_add(io, e_io_handle_events, EVENT_WRITE);
  }
  if (nwrite < len) {
    if (io->write_bufsize + len - nwrite > EVENT_MAX_WRITE_BUFSIZE) {
      if (io->write_bufsize > EVENT_MAX_WRITE_BUFSIZE) {
        fprintf(stderr, "write bufsize > %u, close it!",
                (unsigned int)EVENT_MAX_WRITE_BUFSIZE);
        goto write_error;
      }
    }
    offset_buf_t remain;
    remain.len = len - nwrite;
    remain.offset = 0;
    // NOTE: free in nio_write
    EVENT_ALLOC(remain.base, remain.len);
    memcpy(remain.base, ((char *)buf) + nwrite, remain.len);
    if (io->write_queue.maxsize == 0) {
      write_queue_init(&io->write_queue, 4);
    }
    write_queue_push_back(&io->write_queue, &remain);
    io->write_bufsize += remain.len;
    if (io->write_bufsize > EVENT_WRITE_BUFSIZE_HIGH_WATER) {
      fprintf(stderr, "write len=%d enqueue %u, bufsize=%u over high water %u",
              len, (unsigned int)(remain.len - remain.offset),
              (unsigned int)io->write_bufsize,
              (unsigned int)EVENT_WRITE_BUFSIZE_HIGH_WATER);
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
   * if hio_close_sync, we have to be very careful to avoid using freed
   * resources. But if hio_close_async, we do not have to worry about this.
   */
  e_io_close_async(io);
  return nwrite < 0 ? nwrite : -1;
}
