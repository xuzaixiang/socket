//
// Created by 20123460 on 2022/1/15.
//

#include "e_socket.h"
#include "event/e_sockaddr.h"

e_io_t *e_socket_create(e_loop_t *loop, const char *host, int port, e_io_type_t type, e_io_side_t side) {
  int sock_type = type & EVENT_IO_TYPE_SOCK_STREAM ? SOCK_STREAM :
                  type & EVENT_IO_TYPE_SOCK_DGRAM  ? SOCK_DGRAM :
                  type & EVENT_IO_TYPE_SOCK_RAW    ? SOCK_RAW : -1;
  if (sock_type == -1) return NULL;
  sockaddr_u addr;
  memset(&addr, 0, sizeof(addr));
  int ret = -1;
#ifdef EVENT_ENABLE_UDS
  if (port < 0) {
        sockaddr_set_path(&addr, host);
        ret = 0;
    }
#endif

}