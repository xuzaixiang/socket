#ifndef EVENT_BUF_H
#define EVENT_BUF_H

#define EVENT_LOOP_READ_BUFSIZE          8192        // 8K
#define EVENT_READ_BUFSIZE_HIGH_WATER     65536       // 64K
#define EVENT_WRITE_BUFSIZE_HIGH_WATER    (1U << 23)  // 8M
#define EVENT_MAX_READ_BUFSIZE            (1U << 24)  // 16M
#define EVENT_MAX_WRITE_BUFSIZE           (1U << 26)  // 64M

typedef struct e_buf_s {
  char *base;
  size_t len;

#ifdef __cplusplus
  e_buf_s() {
        base = NULL;
        len  = 0;
    }

    e_buf_s(void* data, size_t len) {
        this->base = (char*)data;
        this->len  = len;
    }
#endif
} e_buf_t;


typedef struct fifo_buf_s {
  char*  base;
  size_t len;
  size_t head;
  size_t tail;
#ifdef __cplusplus
  fifo_buf_s() {
        base = NULL;
        len = 0;
        head = tail = 0;
    }

    fifo_buf_s(void* data, size_t len) {
        this->base = (char*)data;
        this->len = len;
        head = tail = 0;
    }
#endif
} fifo_buf_t;

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

#endif