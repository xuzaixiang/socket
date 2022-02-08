//
// Created by 20123460 on 2022/1/17.
//
#include "e_math.h"

unsigned long ceil2e(unsigned long num) {
  // 2**0 = 1
  if (num == 0 || num == 1)   return 1;
  unsigned long n = num - 1;
  int e = 1;
  while (n>>=1) ++e;
  unsigned long ret = 1;
  while (e--) ret<<=1;
  return ret;
}

long long varint_decode(const unsigned char* buf, int* len){
  long long ret = 0;
  int bytes = 0, bits = 0;
  const unsigned char *p = buf;
  do {
    if (len && *len && bytes == *len) {
      // Not enough length
      *len = 0;
      return 0;
    }
    ret |= ((long long)(*p & 0x7F)) << bits;
    ++bytes;
    if ((*p & 0x80) == 0) {
      // Found end
      if (len) *len = bytes;
      return ret;
    }
    ++p;
    bits += 7;
  } while(bytes < 10);

  // Not found end
  if (len) *len = -1;
  return ret;
}