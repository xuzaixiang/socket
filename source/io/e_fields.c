//
// Created by 20123460 on 2022/1/19.
//

#include "event/e_io.h"

void e_io_setcb_accept(e_io_t *io, e_accept_cb accept_cb){
  io->accept_cb = accept_cb;
}