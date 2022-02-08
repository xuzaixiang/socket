//
// Created by 20123460 on 2022/2/8.
//

#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include "e_def.h"

#define EVENT_LOOP_FLAG_RUN_ONCE                    0x00000001
#define EVENT_LOOP_FLAG_AUTO_FREE                   0x00000002
#define EVENT_LOOP_FLAG_QUIT_WHEN_NO_ACTIVE_EVENTS  0x00000004

BEGIN_EXTERN_C

//HV_EXPORT hloop_t* hloop_new(int flags DEFAULT(HLOOP_FLAG_AUTO_FREE));

END_EXTERN_C

#endif// EVENT_LOOP_H
