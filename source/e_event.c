//
// Created by 20123460 on 2022/1/20.
//

#include "event/e_event.h"
#include <stdatomic.h>

uint64_t e_event_next_id(){
  static atomic_long s_id = 0;
  return ++s_id;
}
