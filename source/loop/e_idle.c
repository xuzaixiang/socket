#include "event/e_loop.h"
#include "event/e_list.h"

e_idle_t* e_idle_add(e_loop_t* loop, e_idle_cb cb, uint32_t repeat DEFAULT(INFINITE)){
  e_idle_t* idle;
  EVENT_ALLOC_SIZEOF(idle);
  idle->event_type = EVENT_TYPE_IDLE;
  idle->priority = EVENT_LOWEST_PRIORITY;
  idle->repeat = repeat;
  list_add(&idle->node, &loop->idles);
  EVENT_ADD(loop, idle, cb);
  loop->nidles++;
  return idle;
}

static void __hidle_del(e_idle_t* idle) {
  if (idle->destroy) return;
  idle->destroy = 1;
  list_del(&idle->node);
  idle->loop->nidles--;
}

void e_idle_del(e_idle_t *idle){
  if (!idle->active) return;
  __hidle_del(idle);
  EVENT_DEL(idle);
}