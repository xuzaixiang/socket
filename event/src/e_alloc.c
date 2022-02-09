//
// Created by 20123460 on 2022/2/9.
//

#include <event/e_alloc.h>

static void *local_malloc(size_t size) { return malloc(size); }
static void *local_realloc(void *ptr, size_t size) {
  return realloc(ptr, size);
}
static void *local_calloc(size_t count, size_t size) {
  return calloc(count, size);
}
static void local_free(void *ptr) { free(ptr); }

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

int e_allocator(e_malloc_func malloc_func, e_realloc_func realloc_func,
                e_calloc_func calloc_func, e_free_func free_func) {
  local_allocator.malloc = malloc_func;
  local_allocator.realloc = realloc_func;
  local_allocator.calloc = calloc_func;
  local_allocator.free = free_func;
  return 0;
}

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
  int saved_errno = errno;
  local_allocator.free(ptr);
  errno = saved_errno;
}