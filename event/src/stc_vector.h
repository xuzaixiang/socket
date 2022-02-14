//
// Created by 20123460 on 2022/2/10.
//

#ifndef STC_VECTOR_H
#define STC_VECTOR_H

#include <assert.h> // for assert
#include <stddef.h> // for size_t,NULL,
#include <stdlib.h> // for malloc,free
#include <string.h> // for memset,memmove

/*
 * like c++ std::vector
 * methods:
 *
 * @effective
 * name_push_back,
 * name_pop_back,
 * name_del_nomove,(note: it will change order)
 * name_swap,
 *
 * @ineffective
 * name_del,
 * name_add,
 *
 * name_new,name_news,name_newsv,
 * name_free,
 * name_clear,
 * name_resize,name_resizev,
 *
 * name_get,name_set,
 * name_front,
 * name_back,
 * name_empty,
 * name_capacity,
 * name_data,
 * name_size,
 */

#define STC_VECTOR(type, name)                                                 \
  struct name {                                                                \
    type *ptr;                                                                 \
    size_t size;                                                               \
    size_t capacity;                                                           \
  };                                                                           \
  typedef struct name name;                                                    \
  static inline void name##_new(name *p) {                                     \
    p->size = p->capacity = 0;                                                 \
    p->ptr = NULL;                                                             \
  }                                                                            \
  static inline int name##_news(name *p, size_t size) {                        \
    assert(size > 0);                                                          \
    size_t mem = sizeof(type) * size;                                          \
    if ((p->ptr = (type *)malloc(mem)) == NULL)                                \
      return -1;                                                               \
    memset(p->ptr, 0, mem);                                                    \
    p->size = p->capacity = size;                                              \
    return 0;                                                                  \
  }                                                                            \
  static inline int name##_newsv(name *p, size_t size, type elem) {            \
    assert(size > 0);                                                          \
    size_t mem = sizeof(type) * size;                                          \
    if ((p->ptr = (type *)malloc(mem)) == NULL)                                \
      return -1;                                                               \
    for (size_t i = 0; i < size; i++)                                          \
      p->ptr[i] = elem;                                                        \
    p->size = p->capacity = size;                                              \
    return 0;                                                                  \
  }                                                                            \
  static inline void name##_free(name *p) {                                    \
    free(p->ptr);                                                              \
    p->ptr = NULL;                                                             \
    p->size = p->capacity = 0;                                                 \
  }                                                                            \
  static inline int name##_resize(name *p, size_t size) {                      \
    if (size > p->capacity) {                                                  \
      if ((p->ptr = (type *)realloc(p->ptr, (p->capacity = size) *             \
                                                sizeof(type))) == NULL)        \
        return -1;                                                             \
    }                                                                          \
    if (size > p->size) {                                                      \
      memset(p->ptr + p->size, 0, (size - p->size) * sizeof(type));            \
    }                                                                          \
    p->size = size;                                                            \
    if (size < (p->capacity >> 2)) {                                           \
      size_t c = p->capacity >> 1;                                             \
      void *r;                                                                 \
      if ((r = realloc(p->ptr, c * sizeof(type)))) {                           \
        p->ptr = (type *)r;                                                    \
        p->capacity = c;                                                       \
      }                                                                        \
    }                                                                          \
    return 0;                                                                  \
  }                                                                            \
  static inline int name##_resizev(name *p, size_t size, type elem) {          \
    if (size > p->capacity) {                                                  \
      if ((p->ptr = (type *)realloc(p->ptr, (p->capacity = size) *             \
                                                sizeof(type))) == NULL)        \
        return -1;                                                             \
    }                                                                          \
    if (size > p->size) {                                                      \
      for (int i = p->size; i < size; i++)                                     \
        p->ptr[i] = elem;                                                      \
    }                                                                          \
    p->size = size;                                                            \
    if (size < (p->capacity >> 2)) {                                           \
      size_t c = p->capacity >> 1;                                             \
      void *r;                                                                 \
      if ((r = realloc(p->ptr, c * sizeof(type)))) {                           \
        p->ptr = (type *)r;                                                    \
        p->capacity = c;                                                       \
      }                                                                        \
    }                                                                          \
    return 0;                                                                  \
  }                                                                            \
  static inline int name##_push_back(name *p, type elem) {                     \
    if (p->size == p->capacity) {                                              \
      if ((p->ptr = (type *)realloc(                                           \
               p->ptr,                                                         \
               (p->capacity = (p->capacity == 0 ? 1 : p->capacity << 1)) *     \
                   sizeof(type))) == NULL)                                     \
        return -1;                                                             \
    }                                                                          \
    p->ptr[p->size] = elem;                                                    \
    p->size++;                                                                 \
    return 0;                                                                  \
  }                                                                            \
  static inline void name##_pop_back(name *p) {                                \
    assert(p->size > 0);                                                       \
    p->size--;                                                                 \
    if (p->size < (p->capacity >> 2)) {                                        \
      size_t c = p->capacity >> 1;                                             \
      void *r;                                                                 \
      if ((r = realloc(p->ptr, c * sizeof(type)))) {                           \
        p->ptr = (type *)r;                                                    \
        p->capacity = c;                                                       \
      }                                                                        \
    }                                                                          \
  }                                                                            \
  static inline int name##_add(name *p, size_t pos, type elem) {               \
    assert(pos <= p->size);                                                    \
    if (p->size == p->capacity) {                                              \
      if ((p->ptr = (type *)realloc(                                           \
               p->ptr,                                                         \
               (p->capacity = (p->capacity == 0 ? 1 : p->capacity << 1)) *     \
                   sizeof(type))) == NULL)                                     \
        return -1;                                                             \
    }                                                                          \
    if (pos < p->size) {                                                       \
      memmove(p->ptr + pos + 1, p->ptr + pos, sizeof(type) * (p->size - pos)); \
    }                                                                          \
    p->ptr[pos] = elem;                                                        \
    p->size++;                                                                 \
    return 0;                                                                  \
  }                                                                            \
  static inline void name##_del(name *p, size_t pos) {                         \
    assert(p->size > 0 && pos < p->size);                                      \
    p->size--;                                                                 \
    if (pos < p->size) {                                                       \
      memmove(p->ptr + pos, p->ptr + pos + 1, sizeof(type) * (p->size - pos)); \
    }                                                                          \
    if (p->size < (p->capacity >> 2)) {                                        \
      size_t c = p->capacity >> 1;                                             \
      void *r;                                                                 \
      if ((r = realloc(p->ptr, c * sizeof(type)))) {                           \
        p->ptr = (type *)r;                                                    \
        p->capacity = c;                                                       \
      }                                                                        \
    }                                                                          \
  }                                                                            \
  static inline void name##_del_nomove(name *p, size_t pos) {                  \
    assert(p->size > 0 && pos < p->size);                                      \
    p->size--;                                                                 \
    if (pos < p->size) {                                                       \
      p->ptr[pos] = p->ptr[p->size];                                           \
    }                                                                          \
    if (p->size < (p->capacity >> 2)) {                                        \
      size_t c = p->capacity >> 1;                                             \
      void *r;                                                                 \
      if ((r = realloc(p->ptr, c * sizeof(type)))) {                           \
        p->ptr = (type *)r;                                                    \
        p->capacity = c;                                                       \
      }                                                                        \
    }                                                                          \
  }                                                                            \
  static inline void name##_swap(name *p, size_t pos1, size_t pos2) {          \
    if (pos1 == pos2)                                                          \
      return;                                                                  \
    assert(pos1 < p->size && pos2 < p->size);                                  \
    type temp = p->ptr[pos1];                                                  \
    p->ptr[pos1] = p->ptr[pos2];                                               \
    p->ptr[pos2] = temp;                                                       \
  }                                                                            \
  static inline type *name##_get(name *p, size_t pos) {                        \
    assert(pos < p->size);                                                     \
    return p->ptr + pos;                                                       \
  }                                                                            \
  static inline void name##_set(name *p, size_t pos, type elem) {              \
    assert(pos < p->size);                                                     \
    p->ptr[pos] = elem;                                                        \
  }                                                                            \
  static inline type name##_front(name *p) {                                   \
    assert(p->size > 0);                                                       \
    return *p->ptr;                                                            \
  }                                                                            \
  static inline type name##_back(name *p) {                                    \
    assert(p->size > 0);                                                       \
    return p->ptr[p->size - 1];                                                \
  }                                                                            \
  static inline void name##_clear(name *p) { p->size = 0; }                    \
  static inline int name##_empty(name *p) { return p->size == 0; }             \
  static inline size_t name##_capacity(name *p) { return p->capacity; }        \
  static inline size_t name##_size(name *p) { return p->size; }                \
  static inline type *name##_data(name *p) { return p->ptr; }

#endif // STC_VECTOR_H
