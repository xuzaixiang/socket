//
// Created by 20123460 on 2022/1/8.
//

#ifndef EVENT_ALLOC_H
#define EVENT_ALLOC_H

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void *e_malloc_safe(size_t size);
void *e_realloc_safe(void *oldptr, size_t newsize, size_t oldsize);
void *e_calloc_safe(size_t nmemb, size_t size);
void *e_zalloc_safe(size_t size);
void e_free_safe(void *ptr);

#ifdef EVENT_ALLOC_COUNT
long e_alloc_count();
long e_free_count();
void e_memcheck();
#define EVENT_MEMCHECK    atexit(e_memcheck)
#define printd(...) printf(__VA_ARGS__)
#else
#define printd(...)
#endif

#define EVENT_ALLOC(ptr, size)\
    do {\
        *(void**)&(ptr) = e_zalloc_safe(size);\
        printd("alloc(%p, size=%llu)\tat [%s:%d:%s]\n", ptr, (unsigned long long)size, __FILE__, __LINE__, __FUNCTION__);\
    } while(0)

#define EVENT_ALLOC_SIZEOF(ptr)  EVENT_ALLOC(ptr, sizeof(*(ptr)))

#define EVENT_FREE(ptr)\
    do {\
        if (ptr) {\
            e_free_safe(ptr);\
            printd("free( %p )\tat [%s:%d:%s]\n", ptr, __FILE__, __LINE__, __FUNCTION__);\
            ptr = NULL;\
        }\
    } while(0)

#ifdef __cplusplus
}
#endif

#endif //EVENT_ALLOC_H
