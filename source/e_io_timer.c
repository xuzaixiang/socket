#include "event/e_io.h"
#include "event/e_loop.h"

void e_io_del_connect_timer(e_io_t* io){
  if (io->connect_timer) {
    e_timer_del(io->connect_timer);
    io->connect_timer = NULL;
    io->connect_timeout = 0;
  }
}

void e_io_del_close_timer(e_io_t* io){
  if (io->close_timer) {
    e_timer_del(io->close_timer);
    io->close_timer = NULL;
    io->close_timeout = 0;
  }
}
void e_io_del_read_timer(e_io_t* io){
  if (io->read_timer) {
    e_timer_del(io->read_timer);
    io->read_timer = NULL;
    io->read_timeout = 0;
  }
}
void e_io_del_write_timer(e_io_t* io){
  if (io->write_timer) {
    e_timer_del(io->write_timer);
    io->write_timer = NULL;
    io->write_timeout = 0;
  }
}
void e_io_del_keepalive_timer(e_io_t* io){
  if (io->keepalive_timer) {
    e_timer_del(io->keepalive_timer);
    io->keepalive_timer = NULL;
    io->keepalive_timeout = 0;
  }
}
void e_io_del_heartbeat_timer(e_io_t* io){
  if (io->heartbeat_timer) {
    e_timer_del(io->heartbeat_timer);
    io->heartbeat_timer = NULL;
    io->heartbeat_interval = 0;
    io->heartbeat_fn = NULL;
  }
}