//
// Created by 20123460 on 2022/1/8.
//

#include <string.h>
#include <errno.h>
#include "event/util/e_alloc.h"

#ifdef EVENT_ALLOC_COUNT
#include <stdatomic.h>
static atomic_long local_alloc_count = 0;
static atomic_long local_free_count = 0;

long e_alloc_count() {
  return local_alloc_count;
}
long e_free_count() {
  return local_free_count;
}
void e_memcheck() {
  printfd("Memcheck => alloc:%ld free:%ld\n", e_alloc_count(), e_free_count());
}
#define e_atomic_inc(value) atomic_fetch_add(value,1)
#else
#define e_atomic_inc(value)
#endif

static void *check_ptr(void *ptr) {
  if (ptr == NULL) {
    fprintfd(stderr, "allocated ptr is null !\n");
    exit(-1);
  }
  return ptr;
}

static void *local_malloc(size_t size) {
  e_atomic_inc(&local_alloc_count);
  return check_ptr(malloc(size));
}
static void *local_realloc(void *ptr, size_t size) {
  if (ptr != NULL)
    e_atomic_inc(&local_free_count);
  if (size > 0)
    e_atomic_inc(&local_alloc_count);
  return check_ptr(realloc(ptr, size));
}
static void *local_calloc(size_t count, size_t size) {
  e_atomic_inc(&local_alloc_count);
  return check_ptr(calloc(count, size));
}
static void local_free(void *ptr) {
  e_atomic_inc(&local_free_count);
  free(ptr);
}

typedef struct {
  e_malloc_func malloc;
  e_realloc_func realloc;
  e_calloc_func calloc;
  e_free_func free;
} e_allocator_t;

static e_allocator_t local_allocator = {
    local_malloc,
    local_realloc,
    local_calloc,
    local_free,
};

void *e_malloc(size_t size) {
  if (size > 0)
    return local_allocator.malloc(size);
  return NULL;
}

void *e_realloc(void *ptr, size_t size) {
  if (size > 0)
    return local_allocator.realloc(ptr, size);
  e_free(ptr);
  return NULL;
}
void *e_calloc(size_t nmemb, size_t size) {
  return local_allocator.calloc(nmemb, size);
}

void *e_zalloc(size_t size) {
  void *ptr = local_allocator.malloc(size);
  memset(ptr, 0, size);
  return ptr;
}

void e_free(void *ptr) {
  int saved_errno;
  saved_errno = errno;
  local_allocator.free(ptr);
  errno = saved_errno;
}