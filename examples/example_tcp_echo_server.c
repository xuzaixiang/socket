
#include "event/e_loop.h"

#define EVENT_ALLOC_COUNT
#include "event/e_alloc.h"


static void on_accept(e_io_t* io) {
//  printf("on_accept connfd=%d\n", e_io_fd(io));
//  char localaddrstr[SOCKADDR_STRLEN] = {0};
//  char peeraddrstr[SOCKADDR_STRLEN] = {0};
//  printf("accept connfd=%d [%s] <= [%s]\n", hio_fd(io),
//         SOCKADDR_STR(hio_localaddr(io), localaddrstr),
//         SOCKADDR_STR(hio_peeraddr(io), peeraddrstr));
//
//  hio_setcb_close(io, on_close);
//  hio_setcb_read(io, on_recv);
//
//#if TEST_UNPACK
//  hio_set_unpack(io, &unpack_setting);
//#endif
//
//#if TEST_READ_ONCE
//  hio_read_once(io);
//#elif TEST_READLINE
//  hio_readline(io);
//#elif TEST_READSTRING
//  hio_readstring(io);
//#elif TEST_READBYTES
//  hio_readbytes(io, TEST_READBYTES);
//#else
//  hio_read_start(io);
//#endif
}

int main(int argc, char** argv){
#ifdef EVENT_ALLOC_COUNT
  E_MEMCHECK;
#endif
  const char* host = "0.0.0.0";
  int port = 8080;
  e_loop_t* loop = e_loop_new(0);
#if TEST_SSL
  hio_t* listenio = hloop_create_ssl_server(loop, host, port, on_accept);
#else
  e_io_t* listenio = e_loop_create_tcp_server(loop, host, port, on_accept);
#endif

  return 0;
}