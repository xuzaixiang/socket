#ifndef EVENT_LIST_H
#define EVENT_LIST_H

#include <stddef.h>

#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

#if __has_builtin(__builtin_prefetch)
#define prefetch(x) __builtin_prefetch(x)
#else
#define prefetch(x) (void)0
#endif

struct list_head {
  struct list_head *next, *prev;
};
#define list_node   list_head

static inline void list_init(struct list_head *list) {
  list->next = list;
  list->prev = list;
}

#endif //EVENT_LIST_H