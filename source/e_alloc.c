//
// Created by 20123460 on 2022/1/8.
//

#include <string.h>
#include "event/e_alloc.h"

#ifdef EVENT_ALLOC_COUNT
#include "event/e_atomic.h"
static e_atomic_t s_alloc_count = E_ATOMIC_VAR_INIT(0);
static e_atomic_t s_free_count = E_ATOMIC_VAR_INIT(0);
long e_alloc_count() {
  return s_alloc_count;
}
long e_free_count() {
  return s_free_count;
}
void e_memcheck() {
  printf("Memcheck => alloc:%ld free:%ld\n", e_alloc_count(), e_free_count());
}
#else
#define e_atomic_inc(value);
#endif

void *e_malloc_safe(size_t size) {
  e_atomic_inc(&s_alloc_count);
  void *ptr = malloc(size);
  if (!ptr) {
    fprintf(stderr, "malloc failed!\n");
    exit(-1);
  }
  return ptr;
}

void *e_realloc_safe(void *oldptr, size_t newsize, size_t oldsize) {
  if (oldptr != NULL) {
    e_atomic_inc(&s_free_count);
  }
  e_atomic_inc(&s_alloc_count);
  void *ptr = realloc(oldptr, newsize);
  if (!ptr) {
    fprintf(stderr, "realloc failed!\n");
    exit(-1);
  }
  if (newsize > oldsize) {
    memset((char *) ptr + oldsize, 0, newsize - oldsize);
  }
  return ptr;
}

void *e_calloc_safe(size_t nmemb, size_t size) {
  e_atomic_inc(&s_alloc_count);
  void *ptr = calloc(nmemb, size);
  if (!ptr) {
    fprintf(stderr, "calloc failed!\n");
    exit(-1);
  }
  return ptr;
}
void *e_zalloc_safe(size_t size) {
  e_atomic_inc(&s_alloc_count);
  void *ptr = malloc(size);
  if (!ptr) {
    fprintf(stderr, "malloc failed!\n");
    exit(-1);
  }
  memset(ptr, 0, size);
  return ptr;
}
void e_free_safe(void *ptr) {
  if (ptr) {
    free(ptr);
    ptr = NULL;
    e_atomic_inc(&s_free_count);
  }
}
