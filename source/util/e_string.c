//
// Created by 20123460 on 2022/1/16.
//
#include <string.h>
#include "event/util/e_string.h"
#include "event/util/e_alloc.h"

char *e_strdup(const char *s) {
  size_t len = strlen(s) + 1;
  char *m = e_malloc(len);
  if (m == NULL)
    return NULL;
  return memcpy(m, s, len);
}

char *e_strndup(const char *s, size_t n) {
  char *m;
  size_t len = strlen(s);
  if (n < len)
    len = n;
  m = e_malloc(len + 1);
  if (m == NULL)
    return NULL;
  m[len] = '\0';
  return memcpy(m, s, len);
}

