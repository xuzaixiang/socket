#ifndef EVENT_IO_TYPE_H
#define EVENT_IO_TYPE_H

#include "../e_export.h"

typedef enum {
  EVENT_IO_TYPE_UNKNOWN    = 0,
  EVENT_IO_TYPE_STDIN      = 0x00000001,//  0000 0001
  EVENT_IO_TYPE_STDOUT     = 0x00000002,//  0000 0010
  EVENT_IO_TYPE_STDERR     = 0x00000004,//  0000 0100
  EVENT_IO_TYPE_STDIO      = 0x0000000F,//  0000 1111

  EVENT_IO_TYPE_FILE       = 0x00000010,//  0001 0000

  EVENT_IO_TYPE_IP         = 0x00000100,//  0001 0000 0000
  EVENT_IO_TYPE_SOCK_RAW   = 0x00000F00,//  1111 0000 0000

  EVENT_IO_TYPE_UDP        = 0x00001000,//  0000 0001 0000 0000 0000
  EVENT_IO_TYPE_KCP        = 0x00002000,//  0000 0010 0000 0000 0000
  EVENT_IO_TYPE_DTLS       = 0x00010000,//  0001 0000 0000 0000 0000
  EVENT_IO_TYPE_SOCK_DGRAM = 0x000FF000,//  1111 1111 0000 0000 0000

  EVENT_IO_TYPE_TCP        = 0x00100000,
  EVENT_IO_TYPE_SSL        = 0x01000000,
  EVENT_IO_TYPE_TLS        = EVENT_IO_TYPE_SSL,
  EVENT_IO_TYPE_SOCK_STREAM= 0x0FF00000,

  EVENT_IO_TYPE_SOCKET     = 0x0FFFFF00,
} e_io_type_e;


typedef enum {
  EVENT_IO_SERVER_SIDE  = 0,
  EVENT_IO_CLIENT_SIDE  = 1,
} e_io_side_e;


typedef struct e_loop_s e_loop_t;
typedef struct e_io_s e_io_t;

// @hio_create_socket: socket -> bind -> listen
// sockaddr_set_ipport -> socket -> hio_get(loop, sockfd) ->
// side == HIO_SERVER_SIDE ? bind ->
// type & HIO_TYPE_SOCK_STREAM ? listen ->
e_io_t *e_io_create_socket(e_loop_t *loop, const char *host, int port,
                           e_io_type_e type DEFAULT(EVENT_IO_TYPE_TCP),
                           e_io_side_e side DEFAULT(EVENT_IO_SERVER_SIDE));



#endif
