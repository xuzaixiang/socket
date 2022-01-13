
#include "event/e_loop.h"


void e_loop_post_event(e_loop_t *loop, e_event_t *ev) {
  if (ev->loop == NULL) {
    ev->loop = loop;
  }
  if (ev->event_type == 0) {
    ev->event_type = EVENT_TYPE_CUSTOM;
  }
  if (ev->event_id == 0) {
    ev->event_id = e_loop_next_event_id();
  }

  int nsend = 0;
  e_mutex_lock(&loop->custom_events_mutex);
  if (loop->sockpair[SOCKPAIR_WRITE_INDEX] == -1) {
    if (e_loop_create_sockpair(loop) != 0) {
      goto unlock;
    }
  }
#if defined(EVENT_OS_LINUX)
  nsend = write(loop->sockpair[SOCKPAIR_WRITE_INDEX], "e", 1);
#else
  nsend =  send(loop->sockpair[SOCKPAIR_WRITE_INDEX], "e", 1, 0);
#endif
  if (nsend != 1) {
//    hloge("send failed!");
    goto unlock;
  }
  e_queue_push_back(&loop->custom_events, ev);
  unlock:
  e_mutex_unlock(&loop->custom_events_mutex);
}