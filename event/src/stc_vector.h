//
// Created by 20123460 on 2022/2/19.
//

#ifndef STC_VECTOR_H
#define STC_VECTOR_H

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct stc_vector_s stc_vector_t;

#define STC_VECTOR(type, name)                                                 \
  static inline stc_vector_t *stc_vector_new_of_##name(size_t capacity) {          \
    return stc_vector_newes(sizeof(type), capacity);                               \
  }                                                                            \
  static inline int stc_vector_push_back_of_##name(stc_vector_t *vector,       \
                                                   type *elem) {               \
    return stc_vector_push_back(vector, elem);                                 \
  }                                                                            \
  static inline int stc_vector_add_of_##name(stc_vector_t *vector,             \
                                             size_t position, type *elem) {    \
    return stc_vector_add(vector, position, elem);                             \
  }                                                                            \
  static inline type stc_vector_front_of_##name(stc_vector_t *vector) {        \
    return *(type *)stc_vector_front(vector);                                  \
  }                                                                            \
  static inline type stc_vector_back_of_##name(stc_vector_t *vector) {         \
    return *(type *)stc_vector_back(vector);                                   \
  }                                                                            \
  static inline type stc_vector_get_of_##name(stc_vector_t *vector,            \
                                              size_t position) {               \
    return *(type *)stc_vector_get(vector, position);                          \
  }                                                                            \
  static inline type *stc_vector_data_of_##name(stc_vector_t *vector) {        \
    return (type *)stc_vector_data(vector);                                    \
  }                                                                            \
  static inline void stc_vector_set_of_##name(stc_vector_t *vector,            \
                                              size_t position, type *elem) {   \
    stc_vector_set(vector, position, elem);                                    \
  }


struct stc_vector_s {
  char *ptr;
  size_t size;
  size_t each;
  size_t capacity;
  size_t min_capacity;
};

static inline stc_vector_t *stc_vector_newes(size_t each, size_t capacity) {
  stc_vector_t *vector;
  if ((vector = (stc_vector_t *)calloc(sizeof(stc_vector_t), 1)) == NULL)
    return NULL;
  vector->each = each;
  vector->min_capacity = capacity;
  if (capacity > 0) {
    if ((vector->ptr = (char *)calloc(each, capacity)) != NULL)
      vector->capacity = capacity;
  }
  return vector;
}
static inline void stc_vector_shrink(stc_vector_t *vector) {
  if (vector->capacity <= vector->min_capacity)
    return;
  if (vector->size < (vector->capacity >> 2)) {
    size_t c = vector->capacity >> 1;
    void *r;
    if ((r = realloc(vector->ptr, c * vector->each))) {
      vector->ptr = (char *)r;
      vector->capacity = c;
    }
  }
}
static inline int stc_vector_expand(stc_vector_t *vector, size_t capacity) {
  if (vector->size == vector->capacity) {
    if ((vector->ptr = (char *)realloc(
             vector->ptr, (vector->capacity = capacity) * vector->each)) ==
        NULL)
      return -1;
  }
  return 0;
}
static inline void stc_vector_free(stc_vector_t **vector) {
  if (vector && *vector) {
    free((*vector)->ptr);
    free(*vector);
    *vector = NULL;
  }
}
static inline int stc_vector_resize(stc_vector_t *vector, size_t size) {
  if (size > vector->capacity) {
    if ((vector->ptr = (char *)realloc(vector->ptr, (vector->capacity = size) *
                                                        vector->each)) == NULL)
      return -1;
  }
  if (size > vector->size) {
    memset(vector->ptr + vector->size * vector->each, 0,
           (size - vector->size) * vector->each);
  }
  vector->size = size;
  stc_vector_shrink(vector);
  return 0;
}
static inline int stc_vector_add(stc_vector_t *vector, size_t position, void *type_pointer) {
  assert(position <= vector->size);
  if (stc_vector_expand(vector,
                        vector->capacity == 0 ? 1 : vector->capacity << 1))
    return -1;
  if (position < vector->size) {
    memmove(vector->ptr + (position + 1) * vector->each,
            vector->ptr + position * vector->each,
            vector->each * (vector->size - position));
  }
  memcpy(vector->ptr + position * vector->each, type_pointer, vector->each);
  vector->size++;
  return 0;
}
static inline int stc_vector_push_back(stc_vector_t *vector, void *type_pointer) {
  return stc_vector_add(vector, vector->size, type_pointer);
}
static inline void stc_vector_del(stc_vector_t *vector, size_t position) {
  assert(vector->size > 0 && position < vector->size);
  vector->size--;
  if (position < vector->size) {
    memmove(vector->ptr + position * vector->each,
            vector->ptr + (position + 1) * vector->each,
            (vector->size - position) * vector->each);
  }
  stc_vector_shrink(vector);
}

static inline void stc_vector_del_nomove(stc_vector_t *vector, size_t position) {
  assert(vector->size > 0 && position < vector->size);
  if (position < vector->size) {
    memcpy(vector->ptr + position * vector->each,
           vector->ptr + (vector->size - 1) * vector->each, vector->each);
  }
  vector->size--;
  stc_vector_shrink(vector);
}
static inline void stc_vector_pop_back(stc_vector_t *vector) {
  assert(vector->size > 0);
  stc_vector_del(vector, vector->size - 1);
}
static inline void stc_vector_swap(stc_vector_t *vector, size_t x, size_t y) {
  if (x == y)
    return;
  assert(x < vector->size && y < vector->size);
  void *temp = malloc(vector->each);
  memcpy(temp, vector->ptr + x * vector->each, vector->each);
  memcpy(vector->ptr + x * vector->each, vector->ptr + y * vector->each,
         vector->each);
  memcpy(vector->ptr + y * vector->each, temp, vector->each);
  free(temp);
}
static inline void *stc_vector_get(stc_vector_t *vector, size_t position) {
  assert(position < vector->size);
  return vector->ptr + position * vector->each;
}
static inline void stc_vector_set(stc_vector_t *vector, size_t position, void *type_pointer) {
  assert(position < vector->size);
  memcpy(vector->ptr + position * vector->each, type_pointer, vector->each);
}
static inline void *stc_vector_front(stc_vector_t *vector) {
  assert(vector->size > 0);
  return vector->ptr;
}
static inline void *stc_vector_back(stc_vector_t *vector) {
  assert(vector->size > 0);
  return vector->ptr + (vector->size - 1) * vector->each;
}
static inline void stc_vector_clear(stc_vector_t *vector) { vector->size = 0; }
static inline int stc_vector_empty(stc_vector_t *vector) { return vector->size == 0; }
static inline size_t stc_vector_size(stc_vector_t *vector) { return vector->size; }
static inline size_t stc_vector_capacity(stc_vector_t *vector) { return vector->capacity; }
static inline void *stc_vector_data(stc_vector_t *vector) { return vector->ptr; }
#endif // STC_VECTOR_H
