//
// Created by 20123460 on 2022/1/12.
//

#include "event/e_loop.h"
#include "util/e_time.h"

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

e_timer_t* e_timer_add_period(e_loop_t* loop, e_timer_cb cb,
                              int8_t minute DEFAULT(0),  int8_t hour  DEFAULT(-1), int8_t day DEFAULT(-1),
                              int8_t week   DEFAULT(-1), int8_t month DEFAULT(-1), uint32_t repeat DEFAULT(INFINITE)){
  if (minute > 59 || hour > 23 || day > 31 || week > 6 || month > 12) {
    return NULL;
  }
  e_period_t* timer;
  EVENT_ALLOC_SIZEOF(timer);
  timer->event_type = EVENT_TYPE_PERIOD;
  timer->priority = EVENT_HIGH_PRIORITY;
  timer->repeat = repeat;
  timer->minute = minute;
  timer->hour   = hour;
  timer->day    = day;
  timer->month  = month;
  timer->week   = week;
  timer->next_timeout = (uint64_t)cron_next_timeout(minute, hour, day, week, month) * 1000000;
  heap_insert(&loop->timers, &timer->node);
  EVENT_ADD(loop, timer, cb);
  loop->ntimers++;
  return (e_timer_t*)timer;
}
void e_timer_reset(e_timer_t* timer){
  if (timer->event_type != EVENT_TYPE_TIMEOUT) {
    return;
  }
  e_loop_t* loop = timer->loop;
  e_timeout_t* timeout = (e_timeout_t*)timer;
  if (timer->destroy) {
    loop->ntimers++;
  } else {
    heap_remove(&loop->timers, &timer->node);
  }
  if (timer->repeat == 0) {
    timer->repeat = 1;
  }
  timer->next_timeout = e_loop_now_hrtime(loop) + (uint64_t)timeout->timeout*1000;
  // NOTE: Limit granularity to 100ms
  if (timeout->timeout >= 1000 && timeout->timeout % 100 == 0) {
    timer->next_timeout = timer->next_timeout / 100000 * 100000;
  }
  heap_insert(&loop->timers, &timer->node);
  EVENT_RESET(timer);
}