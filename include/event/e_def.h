#ifndef EVENT_DEF_H
#define EVENT_DEF_H

#ifndef INFINITE
#define INFINITE    (uint32_t)-1
#endif

#ifndef ABS
#define ABS(n)  ((n) > 0 ? (n) : -(n))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef container_of
#define container_of(ptr, type, member) \
((type*)((char*)(ptr) - offsetof(type, member)))
#endif

#endif //EVENT_DEF_H