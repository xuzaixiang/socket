//
// Created by 20123460 on 2022/1/12.
//

#include "event/e_loop_timer.h"
#include "event/e_loop.h"

static void __htimer_del(e_timer_t* timer) {
  if (timer->destroy) return;
  heap_remove(&timer->loop->timers, &timer->node);
  timer->loop->ntimers--;
  timer->destroy = 1;
}

e_timer_t* e_timer_add(e_loop_t* loop, e_timer_cb cb, uint32_t timeout, uint32_t repeat) {
  if (timeout == 0)   return NULL;
  e_timeout_t* timer;
  EVENT_ALLOC_SIZEOF(timer);
  timer->event_type = EVENT_TYPE_TIMEOUT;
  timer->priority = EVENT_HIGHEST_PRIORITY;
  timer->repeat = repeat;
  timer->timeout = timeout;
  e_loop_update_time(loop);
  timer->next_timeout = e_loop_now_hrtime(loop) + (uint64_t)timeout*1000;
  // NOTE: Limit granularity to 100ms
  if (timeout >= 1000 && timeout % 100 == 0) {
    timer->next_timeout = timer->next_timeout / 100000 * 100000;
  }
  heap_insert(&loop->timers, &timer->node);
  EVENT_ADD(loop, timer, cb);
  loop->ntimers++;
  return (e_timer_t*)timer;
}

void e_timer_del(e_timer_t* timer){
  if (!timer->active) return;
  __htimer_del(timer);
  EVENT_DEL(timer);
}