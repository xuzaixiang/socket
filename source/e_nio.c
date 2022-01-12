//
// Created by 20123460 on 2022/1/12.
//
#include "event/e_nio.h"
#include "event/e_io.h"
#include "event/e_loop.h"
#include "e_watcher.h"

static void __connect_timeout_cb(e_timer_t* timer) {
  e_io_t* io = (e_io_t*)timer->privdata;
  if (io) {
    char localaddrstr[SOCKADDR_STRLEN] = {0};
    char peeraddrstr[SOCKADDR_STRLEN] = {0};
//    hlogw("connect timeout [%s] <=> [%s]",
//          SOCKADDR_STR(io->localaddr, localaddrstr),
//          SOCKADDR_STR(io->peeraddr, peeraddrstr));
    io->error = ETIMEDOUT;
    e_io_close(io);
  }
}
static void __connect_cb(e_io_t* io) {
  e_io_del_connect_timer(io);
  e_io_connect_cb(io);
}

static int __e_io_read(e_io_t* io, void* buf, int len) {
  int nread = 0;
  switch (io->io_type) {
    case EVENT_IO_TYPE_SSL:
//      nread = hssl_read(io->ssl, buf, len);
      break;
    case EVENT_IO_TYPE_TCP:
#ifdef EVENT_OS_LINUX
      nread = read(io->fd, buf, len);
#else
      nread = recv(io->fd, buf, len, 0);
#endif
      break;
    case EVENT_IO_TYPE_UDP:
    case EVENT_IO_TYPE_KCP:
    case EVENT_IO_TYPE_IP:
    {
      socklen_t addrlen = sizeof(sockaddr_u);
      nread = recvfrom(io->fd, buf, len, 0, io->peeraddr, &addrlen);
    }
      break;
    default:
      nread = read(io->fd, buf, len);
      break;
  }
  // hlogd("read retval=%d", nread);
  return nread;
}

static void __accept_cb(e_io_t *io) {
  e_io_accept_cb(io);
}

static void __read_cb(e_io_t* io, void* buf, int readbytes) {
  // printd("> %.*s\n", readbytes, buf);
  io->last_read_hrtime = io->loop->cur_hrtime;
  e_io_handle_read(io, buf, readbytes);
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

static void __write_cb(e_io_t *io, const void *buf, int writebytes) {
  // printd("< %.*s\n", writebytes, buf);
  io->last_write_hrtime = io->loop->cur_hrtime;
  e_io_write_cb(io, buf, writebytes);
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

static void nio_write(e_io_t* io) {
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
  offset_buf_t* pbuf = write_queue_front(&io->write_queue);
  char* base = pbuf->base;
  char* buf = base + pbuf->offset;
  int len = pbuf->len - pbuf->offset;
  nwrite = __nio_write(io, buf, len);
  // printd("write retval=%d\n", nwrite);
  if (nwrite < 0) {
    err = socket_errno();
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


static void nio_connect(e_io_t* io) {
  // printd("nio_connect connfd=%d\n", io->fd);
  socklen_t addrlen = sizeof(sockaddr_u);
  int ret = getpeername(io->fd, io->peeraddr, &addrlen);
  if (ret < 0) {
    io->error = socket_errno();
    printd("connect failed: %s: %d\n", strerror(io->error), io->error);
    goto connect_failed;
  }
  else {
    addrlen = sizeof(sockaddr_u);
    getsockname(io->fd, io->localaddr, &addrlen);

    if (io->io_type == EVENT_IO_TYPE_SSL) {
//      if (io->ssl == NULL) {
//        hssl_ctx_t ssl_ctx = hssl_ctx_instance();
//        if (ssl_ctx == NULL) {
//          goto connect_failed;
//        }
//        hssl_t ssl = hssl_new(ssl_ctx, io->fd);
//        if (ssl == NULL) {
//          goto connect_failed;
//        }
//        io->ssl = ssl;
//      }
//      ssl_client_handshake(io);
    }
    else {
      // NOTE: SSL call connect_cb after handshake finished
      __connect_cb(io);
    }

    return;
  }

  connect_failed:
  e_io_close(io);
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

static void nio_read(e_io_t *io) {
  // printd("nio_read fd=%d\n", io->fd);
  void *buf;
  int len = 0, nread = 0, err = 0;
  read:
  buf = io->readbuf.base + io->readbuf.tail;
  if (io->read_flags & EVENT_IO_READ_UNTIL_LENGTH) {
    len = io->read_until_length - (io->readbuf.tail - io->readbuf.head);
  } else {
    len = io->readbuf.len - io->readbuf.tail;
  }
  assert(len > 0);
  nread = __e_io_read(io, buf, len);
  // printd("read retval=%d\n", nread);
  if (nread < 0) {
    err = socket_errno();
    if (err == EAGAIN) {
      // goto read_done;
      return;
    } else if (err == EMSGSIZE) {
      // ignore
      return;
    } else {
      // perror("read");
      io->error = err;
      goto read_error;
    }
  }
  if (nread == 0) {
    goto disconnect;
  }
  io->readbuf.tail += nread;
  __read_cb(io, buf, nread);
  // if (nread == len) goto read;
  return;
  read_error:
  disconnect:
  e_io_close(io);
}
static void e_io_handle_events(e_io_t *io) {
  if ((io->events & EVENT_READ) && (io->revents & EVENT_READ)) {
    if (io->accept) {
      nio_accept(io);
    } else {
      nio_read(io);
    }
  }

  if ((io->events & EVENT_WRITE) && (io->revents & EVENT_WRITE)) {
    // NOTE: del HV_WRITE, if write_queue empty
    e_recursive_mutex_lock(&io->write_mutex);
    if (write_queue_empty(&io->write_queue)) {
      iowatcher_del_event(io->loop, io->fd, EVENT_WRITE);
      io->events &= ~EVENT_WRITE;
    }
    e_recursive_mutex_unlock(&io->write_mutex);
    if (io->connect) {
      // NOTE: connect just do once
      // ONESHOT
      io->connect = 0;

      nio_connect(io);
    }
    else {
      nio_write(io);
    }
  }

  io->revents = 0;
}

int e_io_connect(e_io_t* io) {
  int ret = connect(io->fd, io->peeraddr, SOCKADDR_LEN(io->peeraddr));
#ifdef OS_WIN
  if (ret < 0 && socket_errno() != WSAEWOULDBLOCK) {
#else
  if (ret < 0 && socket_errno() != EINPROGRESS) {
#endif
    perror("connect");
    e_io_close(io);
    return ret;
  }
  if (ret == 0) {
    // connect ok
    nio_connect(io);
    return 0;
  }
  int timeout = io->connect_timeout ? io->connect_timeout : EVENT_IO_DEFAULT_CONNECT_TIMEOUT;
  io->connect_timer = e_timer_add(io->loop, __connect_timeout_cb, timeout, 1);
  io->connect_timer->privdata = io;
  io->connect = 1;
  return hio_add(io, e_io_handle_events, EVENT_WRITE);
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

int e_io_accept(e_io_t *io) {
  io->accept = 1;
  e_io_add(io, e_io_handle_events, EVENT_READ);
  return 0;
}