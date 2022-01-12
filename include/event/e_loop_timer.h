//
// Created by 20123460 on 2022/1/12.
//

#ifndef EVENT_TIMER_H
#define EVENT_TIMER_H

#include "e_event.h"
#include "e_export.h"

e_timer_t* e_timer_add(e_loop_t* loop, e_timer_cb cb, uint32_t timeout, uint32_t repeat DEFAULT(INFINITE));
void e_timer_del(e_timer_t* timer);

#endif //EVENT_TIMER_H
