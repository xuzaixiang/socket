//
// Created by 20123460 on 2022/1/8.
//

#ifndef EVENT_ALLOC_H
#define EVENT_ALLOC_H

#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *(*e_malloc_func)(size_t size);
typedef void *(*e_realloc_func)(void *ptr, size_t size);
typedef void *(*e_calloc_func)(size_t count, size_t size);
typedef void (*e_free_func)(void *ptr);

void *e_malloc(size_t size);
void *e_realloc(void *ptr, size_t size);
void *e_calloc(size_t nmemb, size_t size);
void *e_zalloc(size_t size);
void e_free(void *ptr);


#ifdef EVENT_ALLOC_COUNT
long e_alloc_count();
long e_free_count();
void e_memcheck();
#define EVENT_MEMCHECK    atexit(e_memcheck)
#endif

#ifdef EVENT_ALLOC_PRINT
#include <stdio.h>
#define printfd(...) printf(__VA_ARGS__)
#define fprintfd(fd, msg) fprintf(fd,msg)
#else
#define printd(...)
#define fprintfd(fd,msg)
#endif

#define EVENT_ALLOC(ptr, size)\
    do {\
        *(void**)&(ptr) = e_zalloc(size);\
        printfd("alloc(%p, size=%llu)\tat [%s:%d:%s]\n", ptr, (unsigned long long)size, __FILE__, __LINE__, __FUNCTION__);\
    } while(0)

#define EVENT_ALLOC_SIZEOF(ptr)  EVENT_ALLOC(ptr, sizeof(*(ptr)))

#define EVENT_FREE(ptr)\
    do {\
        if (ptr) {\
            e_free(ptr);\
            printfd("free( %p )\tat [%s:%d:%s]\n", ptr, __FILE__, __LINE__, __FUNCTION__);\
            ptr = NULL;\
        }\
    } while(0)

#ifdef __cplusplus
}
#endif

#endif //EVENT_ALLOC_H