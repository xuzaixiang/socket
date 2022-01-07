#ifndef EVENT_ELOOP_H
#define EVENT_ELOOP_H

typedef struct eloop_s eloop_t;

typedef enum {
    ELOOP_STATUS_STOP,
    ELOOP_STATUS_RUNNING,
    ELOOP_STATUS_PAUSE
} eloop_status_e;

#endif //EVENT_ELOOP_H
