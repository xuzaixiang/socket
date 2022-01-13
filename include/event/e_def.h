#ifndef EVENT_DEF_H
#define EVENT_DEF_H

#define SOCKPAIR_READ_INDEX     0
#define SOCKPAIR_WRITE_INDEX    1

#ifndef INFINITE
#define INFINITE    (uint32_t)-1
#endif

#ifndef ABS
#define ABS(n)  ((n) > 0 ? (n) : -(n))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef LIMIT
#define LIMIT(lower, v, upper) ((v) < (lower) ? (lower) : (v) > (upper) ? (upper) : (v))
#endif


#ifndef container_of
#define container_of(ptr, type, member) \
((type*)((char*)(ptr) - offsetof(type, member)))
#endif

#endif //EVENT_DEF_H