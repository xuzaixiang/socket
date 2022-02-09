//
// Created by 20123460 on 2022/2/9.
//

#ifndef EVENT_ALLOC_H
#define EVENT_ALLOC_H

#include "e_def.h"
#include "e_include.h"

BEGIN_EXTERN_C

typedef void *(*e_malloc_func)(size_t size);
typedef void *(*e_realloc_func)(void *ptr, size_t size);
typedef void *(*e_calloc_func)(size_t count, size_t size);
typedef void (*e_free_func)(void *ptr);

EVENT_EXPORT int e_allocator(e_malloc_func malloc_func,
                             e_realloc_func realloc_func,
                             e_calloc_func calloc_func, e_free_func free_func);
EVENT_EXPORT void *e_malloc(size_t size);
EVENT_EXPORT void *e_realloc(void *ptr, size_t size);
EVENT_EXPORT void *e_calloc(size_t nmemb, size_t size);
EVENT_EXPORT void *e_zalloc(size_t size);
EVENT_EXPORT void e_free(void *ptr);

#define EVENT_ALLOC(ptr, size)                                                 \
  do {                                                                         \
    *(void **)&(ptr) = e_zalloc(size);                                         \
  } while (0)

#define EVENT_ALLOC_SIZEOF(ptr) EVENT_ALLOC(ptr, sizeof(*(ptr)))

#define EVENT_FREE(ptr)                                                        \
  do {                                                                         \
    if (ptr) {                                                                 \
      e_free(ptr);                                                             \
      (ptr) = NULL;                                                            \
    }                                                                          \
  } while (0)

END_EXTERN_C

#endif // EVENT_ALLOC_H
