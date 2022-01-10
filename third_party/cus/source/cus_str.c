
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include "cus/cus_str.h"

char *cus_str_upper(char *str) {
  char *p = str;
  while (*p != '\0') {
    if (*p >= 'a' && *p <= 'z') {
      *p &= ~0x20;
    }
    ++p;
  }
  return str;
}

char *cus_str_lower(char *str) {
  char *p = str;
  while (*p != '\0') {
    if (*p >= 'A' && *p <= 'Z') {
      *p |= 0x20;
    }
    ++p;
  }
  return str;
}

char *cus_str_reverse(char *str) {
  if (str == NULL) return NULL;
  char *b = str;
  char *e = str;
  while (*e) { ++e; }
  --e;
  char tmp;
  while (e > b) {
    tmp = *e;
    *e = *b;
    *b = tmp;
    --e;
    ++b;
  }
  return str;
}

bool cus_str_startswith(const char *str, const char *start) {
  while (*str && *start && *str == *start) {
    ++str;
    ++start;
  }
  return *start == '\0';
}
bool cus_str_endswith(const char *str, const char *end) {
  assert(str != NULL && end != NULL);
  int len1 = 0;
  int len2 = 0;
  while (*str) {
    ++str;
    ++len1;
  }
  while (*end) {
    ++end;
    ++len2;
  }
  if (len1 < len2) return false;
  while (len2-- > 0) {
    --str;
    --end;
    if (*str != *end) {
      return false;
    }
  }
  return true;
}
bool cus_str_contains(const char *str, const char *sub) {
  assert(str != NULL && sub != NULL);
  return strstr(str, sub) != NULL;
}
char *cus_strncpy(char *dest, const char *src, size_t n) {
  assert(dest != NULL && src != NULL);
  char *ret = dest;
  while (*src != '\0' && --n > 0) {
    *dest++ = *src++;
  }
  *dest = '\0';
  return ret;
}
char *cus_strncat(char *dest, const char *src, size_t n) {
  assert(dest != NULL && src != NULL);
  char *ret = dest;
  while (*dest) {
    if (--n > 0) {
      ++dest;
    }else{
      return ret;
    }
  }
  while (*src != '\0' && --n > 0) {
    *dest++ = *src++;
  }
  *dest = '\0';
  return ret;
}