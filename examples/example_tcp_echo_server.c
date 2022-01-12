
#include <unistd.h>
#include "event/e_loop.h"
#include "event/e_io.h"

#define EVENT_ALLOC_COUNT
#include <event/e_alloc.h>


static void on_close(e_io_t* io) {
  printf("on_close fd=%d error=%d\n", e_io_fd(io), e_io_error(io));
}

static void on_recv(e_io_t* io, void* buf, int readbytes) {
  printf("on_recv fd=%d readbytes=%d\n", e_io_fd(io), readbytes);
  char localaddrstr[SOCKADDR_STRLEN] = {0};
  char peeraddrstr[SOCKADDR_STRLEN] = {0};
  printf("[%s] <=> [%s]\n",
         SOCKADDR_STR(e_io_localaddr(io), localaddrstr),
         SOCKADDR_STR(e_io_peeraddr(io), peeraddrstr));
  printf("< %.*s", readbytes, (char*)buf);
  // echo
  printf("> %.*s", readbytes, (char*)buf);
  e_io_write(io, buf, readbytes);

#if TEST_READ_STOP
  hio_read_stop(io);
#elif TEST_READ_ONCE
  hio_read_once(io);
#elif TEST_READLINE
  hio_readline(io);
#elif TEST_READSTRING
  hio_readstring(io);
#elif TEST_READBYTES
  hio_readbytes(io, TEST_READBYTES);
#endif
}

static void on_accept(e_io_t* io) {
  printf("on_accept connfd=%d\n", e_io_fd(io));
  char localaddrstr[SOCKADDR_STRLEN] = {0};
  char peeraddrstr[SOCKADDR_STRLEN] = {0};
  printf("accept connfd=%d [%s] <= [%s]\n", e_io_fd(io),
         SOCKADDR_STR(e_io_localaddr(io), localaddrstr),
         SOCKADDR_STR(e_io_peeraddr(io), peeraddrstr));

  e_io_setcb_close(io, on_close);
  e_io_setcb_read(io, on_recv);

#if TEST_UNPACK
  hio_set_unpack(io, &unpack_setting);
#endif

#if TEST_READ_ONCE
  hio_read_once(io);
#elif TEST_READLINE
  hio_readline(io);
#elif TEST_READSTRING
  hio_readstring(io);
#elif TEST_READBYTES
  hio_readbytes(io, TEST_READBYTES);
#else
  e_io_read_start(io);
#endif
}

int main(int argc, char** argv){
//#ifdef EVENT_ALLOC_COUNT
//  EVENT_MEMCHECK;
//#endif
  const char* host = "0.0.0.0";
  int port = 8081;
  e_loop_t* loop = e_loop_new(0);
#if TEST_SSL
  hio_t* listenio = hloop_create_ssl_server(loop, host, port, on_accept);
#else
  e_io_t* listenio = e_loop_create_tcp_server(loop, host, port, on_accept);
#endif
  if (listenio == NULL) {
    return -20;
  }
  e_loop_run(loop);
  e_loop_free(&loop);
  return 0;
}