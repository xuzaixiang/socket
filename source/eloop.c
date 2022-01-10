
#include "event/eloop.h"
#include "event/ealloc.h"
#include "eevent.h"

eloop_t *eloop_new(int flags DEFAULT(ELOOP_FLAG_AUTO_FREE)) {
  eloop_t *loop;
  E_ALLOC_SIZEOF(loop);
  return loop;
}