//
// Created by 20123460 on 2022/1/17.
//

#ifndef EVENT_BUF_H
#define EVENT_BUF_H

#include <stddef.h>

typedef struct offset_buf_s {
  char*   base;
  size_t  len;
  size_t  offset;
#ifdef __cplusplus
  offset_buf_s() {
        base = NULL;
        len = 0;
        offset = 0;
    }

    offset_buf_s(void* data, size_t len) {
        this->base = (char*)data;
        this->len = len;
        offset = 0;
    }
#endif
} offset_buf_t;

#endif //EVENT_BUF_H
