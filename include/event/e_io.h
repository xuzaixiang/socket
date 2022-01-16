#ifndef EVENT_IO_H
#define EVENT_IO_H

typedef enum e_io_type_e{
  EVENT_IO_TYPE_UNKNOWN    = 0,
  EVENT_IO_TYPE_STDIN      = 0x00000001,
  EVENT_IO_TYPE_STDOUT     = 0x00000002,
  EVENT_IO_TYPE_STDERR     = 0x00000004,
  EVENT_IO_TYPE_STDIO      = 0x0000000F,

  EVENT_IO_TYPE_FILE       = 0x00000010,

  EVENT_IO_TYPE_IP         = 0x00000100,
  EVENT_IO_TYPE_SOCK_RAW   = 0x00000F00,

  EVENT_IO_TYPE_UDP        = 0x00001000,
  EVENT_IO_TYPE_KCP        = 0x00002000,
  EVENT_IO_TYPE_DTLS       = 0x00010000,
  EVENT_IO_TYPE_SOCK_DGRAM = 0x000FF000,

  EVENT_IO_TYPE_TCP        = 0x00100000,
  EVENT_IO_TYPE_SSL        = 0x01000000,
  EVENT_IO_TYPE_TLS        = EVENT_IO_TYPE_SSL,
  EVENT_IO_TYPE_SOCK_STREAM= 0x0FF00000,

  EVENT_IO_TYPE_SOCKET     = 0x0FFFFF00,
} e_io_type_t;

typedef enum e_io_side_e{
  EVENT_IO_SERVER_SIDE  = 0,
  EVENT_IO_CLIENT_SIDE  = 1,
} e_io_side_t;

typedef struct e_io_s {
  int         events;
  int         revents;
} e_io_t;


#endif