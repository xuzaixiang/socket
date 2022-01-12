//
// Created by 20123460 on 2022/1/7.
//

#include "event.h"
#include "event/e_atomic.h"

uint64_t e_loop_next_event_id(){
  static e_atomic_t s_id = EVENT_ATOMIC_VAR_INIT(0);
  return ++s_id;
}