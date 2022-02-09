//
// Created by 20123460 on 2022/2/9.
//

#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

/*
 * queue
 * FIFO: push_back,pop_front
 * stack
 * LIFO: push_back,pop_back
 */

#include <assert.h> // for assert
#include <stddef.h> // for NULL
#include <stdlib.h> // for malloc,realloc,free
#include <string.h> // for memset,memmove

#define QUEUE_INIT_SIZE 16

// #include <deque>
// typedef std::deque<type> qtype;
#define EVENT_QUEUE_DECL(type, qtype)                                          \
  struct qtype {                                                               \
    type *ptr;                                                                 \
    size_t size;                                                               \
    size_t maxsize;                                                            \
    size_t _offset;                                                            \
  };                                                                           \
  typedef struct qtype qtype;                                                  \
                                                                               \
  static inline type *qtype##_data(qtype *p) { return p->ptr + p->_offset; }   \
                                                                               \
  static inline int qtype##_size(qtype *p) { return p->size; }                 \
                                                                               \
  static inline int qtype##_maxsize(qtype *p) { return p->maxsize; }           \
                                                                               \
  static inline int qtype##_empty(qtype *p) { return p->size == 0; }           \
                                                                               \
  static inline type *qtype##_front(qtype *p) {                                \
    return p->size == 0 ? NULL : p->ptr + p->_offset;                          \
  }                                                                            \
                                                                               \
  static inline type *qtype##_back(qtype *p) {                                 \
    return p->size == 0 ? NULL : p->ptr + p->_offset + p->size - 1;            \
  }                                                                            \
                                                                               \
  static inline void qtype##_init(qtype *p, int maxsize) {                     \
    p->_offset = 0;                                                            \
    p->size = 0;                                                               \
    p->maxsize = maxsize;                                                      \
    int alloc_size = sizeof(type) * maxsize;                                   \
    p->ptr = malloc(alloc_size);                                               \
    memset(p->ptr, 0, alloc_size);                                             \
  }                                                                            \
                                                                               \
  static inline void qtype##_clear(qtype *p) {                                 \
    p->_offset = 0;                                                            \
    p->size = 0;                                                               \
    memset(p->ptr, 0, sizeof(type) * p->maxsize);                              \
  }                                                                            \
                                                                               \
  static inline void qtype##_cleanup(qtype *p) {                               \
    free(p->ptr);                                                              \
    p->_offset = p->size = p->maxsize = 0;                                     \
  }                                                                            \
                                                                               \
  static inline void qtype##_resize(qtype *p, int maxsize) {                   \
    if (maxsize == 0)                                                          \
      maxsize = QUEUE_INIT_SIZE;                                               \
    int new_size = sizeof(type) * maxsize;                                     \
    int old_size = sizeof(type) * p->maxsize;                                  \
    p->ptr = (type *)realloc(p->ptr, new_size);                                \
    if (new_size > old_size) {                                                 \
      memset(p->ptr + old_size, 0, new_size - old_size);                       \
    }                                                                          \
    p->maxsize = maxsize;                                                      \
  }                                                                            \
                                                                               \
  static inline void qtype##_double_resize(qtype *p) {                         \
    qtype##_resize(p, p->maxsize * 2);                                         \
  }                                                                            \
                                                                               \
  static inline void qtype##_push_back(qtype *p, type *elem) {                 \
    if (p->size == p->maxsize) {                                               \
      qtype##_double_resize(p);                                                \
    } else if (p->_offset + p->size == p->maxsize) {                           \
      memmove(p->ptr, p->ptr + p->_offset, sizeof(type) * p->size);            \
      p->_offset = 0;                                                          \
    }                                                                          \
    p->ptr[p->_offset + p->size] = *elem;                                      \
    p->size++;                                                                 \
  }                                                                            \
  static inline void qtype##_pop_front(qtype *p) {                             \
    assert(p->size > 0);                                                       \
    p->size--;                                                                 \
    if (++p->_offset == p->maxsize)                                            \
      p->_offset = 0;                                                          \
  }                                                                            \
                                                                               \
  static inline void qtype##_pop_back(qtype *p) {                              \
    assert(p->size > 0);                                                       \
    p->size--;                                                                 \
  }

#endif // EVENT_QUEUE_H
