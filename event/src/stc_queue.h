//
// Created by 20123460 on 2022/2/16.
//

#ifndef STC_QUEUE_H
#define STC_QUEUE_H

#include <assert.h> // for assert,
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/*
 * name: queue
 * feature: FIFO
 * struct: linked list
 */
typedef struct stc_queue_s stc_queue_t;

#define STC_QUEUE(type, name)                                                  \
  static inline stc_queue_t *stc_queue_new_of_##name() {                       \
    return stc_queue_newe(sizeof(type));                                       \
  }                                                                            \
  static inline int stc_queue_push_of_##name(stc_queue_t *queue, type *elem) { \
    return stc_queue_push(queue, elem);                                        \
  }                                                                            \
  static inline void stc_queue_pop_of_##name(stc_queue_t *queue) {             \
    stc_queue_pop(queue);                                                      \
  }                                                                            \
  static inline type stc_queue_front_of_##name(stc_queue_t *queue) {           \
    return *(type *)stc_queue_front(queue);                                    \
  }                                                                            \
  static inline type stc_queue_back_of_##name(stc_queue_t *queue) {            \
    return *(type *)stc_queue_back(queue);                                     \
  }


typedef struct stc_queue_node_s stc_queue_node_t;
#define stc_queue_node_data(node) ((char *)(node) + sizeof(stc_queue_node_t))
#define stc_queue_node_size(queue) ((queue)->each + sizeof(stc_queue_node_t))

struct stc_queue_node_s {
  struct stc_queue_node_s *next;
  // void* data
};

struct stc_queue_s {
  size_t each;
  stc_queue_node_t *head;
  stc_queue_node_t *tail;
  size_t size;
  stc_queue_node_t *open_list;
  size_t open_size;
};

static inline stc_queue_t *stc_queue_newe(size_t each) {
  stc_queue_t *queue;
  if ((queue = (stc_queue_t *)calloc(sizeof(stc_queue_t), 1)) == NULL)
    return NULL;
  queue->each = each;
  return queue;
}

static inline int stc_queue_push(stc_queue_t *queue, void *type_pointer) {
  stc_queue_node_t *node;
  if (queue->open_list != NULL) {
    node = queue->open_list;
    queue->open_list = node->next;
    queue->open_size--;
  } else {
    if ((node = (stc_queue_node_t *)calloc(stc_queue_node_size(queue), 1)) ==
        NULL)
      return -1;
  }
  if (queue->tail) {
    queue->tail->next = node;
    queue->tail = node;
  } else {
    queue->head = queue->tail = node;
  }
  memcpy(stc_queue_node_data(node), type_pointer, queue->each);
  queue->size++;
  return 0;
}
static inline void stc_queue_free_node(stc_queue_node_t *node) {
  while (node) {
    stc_queue_node_t *next = node->next;
    free(node);
    node = next;
  }
}
static inline void stc_queue_free(stc_queue_t **queue) {
  if (queue && *queue) {
    stc_queue_t *p = *queue;
    stc_queue_free_node(p->open_list);
    stc_queue_free_node(p->head);
    p->size = 0;
    p->head = NULL;
    p->open_size = 0;
    p->open_list = NULL;
    free(p);
    *queue = NULL;
  }
}
static inline void stc_queue_pop(stc_queue_t *queue) {
  assert(queue->size > 0);
  stc_queue_node_t *node = queue->head;
  queue->head = node->next;
  if (queue->head == NULL)
    queue->tail = NULL;
  if (queue->open_size < queue->size) {
    if (queue->open_list) {
      queue->open_list->next = node;
    }
    queue->open_list = node;
    node->next = NULL;
    queue->open_size++;
  } else {
    free(node);
  }
  queue->size--;
}
static inline void *stc_queue_front(stc_queue_t *queue) {
  assert(queue->size > 0);
  return stc_queue_node_data(queue->head);
}
static inline void *stc_queue_back(stc_queue_t *queue) {
  assert(queue->size > 0);
  return stc_queue_node_data(queue->tail);
}
static inline int stc_queue_empty(stc_queue_t *queue) { return queue->size == 0; }
static inline size_t stc_queue_size(stc_queue_t *queue) { return queue->size; }
#endif // STC_QUEUE_H
