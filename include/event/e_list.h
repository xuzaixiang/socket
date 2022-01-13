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
static inline void __list_del(struct list_head *prev, struct list_head *next) {
  next->prev = prev;
  prev->next = next;
}
static inline void list_del(struct list_head *entry) {
  __list_del(entry->prev, entry->next);
  //entry->next = NULL;
  //entry->prev = NULL;
}

static inline void __list_add(struct list_head *n,
                              struct list_head *prev,
                              struct list_head *next) {
  next->prev = n;
  n->next = next;
  n->prev = prev;
  prev->next = n;
}
static inline void list_add(struct list_head *n, struct list_head *head) {
  __list_add(n, head, head->next);
}

#endif //EVENT_LIST_H