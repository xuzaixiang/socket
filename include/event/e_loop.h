//
// Created by 20123460 on 2022/1/13.
//

#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include "e_def.h"
#include <stdint.h>//c99

typedef struct e_loop_s {
  uint32_t    flags;
} e_loop_t;

#define EVENT_LOOP_FLAG_RUN_ONCE                     0x00000001
#define EVENT_LOOP_FLAG_AUTO_FREE                    0x00000002
#define EVENT_LOOP_FLAG_QUIT_WHEN_NO_ACTIVE_EVENTS   0x00000004
EVENT_EXPORT e_loop_t* e_loop_new(uint32_t flags DEFAULT(EVENT_LOOP_FLAG_AUTO_FREE));

#endif //EVENT_LOOP_H
