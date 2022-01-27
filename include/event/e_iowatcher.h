#ifndef EVENT_IOWATCHER_H
#define EVENT_IOWATCHER_H

typedef struct e_loop_s e_loop_t;

int e_iowatcher_init        (e_loop_t* loop);
int e_iowatcher_cleanup     (e_loop_t* loop);
int e_iowatcher_add_event   (e_loop_t* loop, int fd, int events);
int e_iowatcher_del_event   (e_loop_t* loop, int fd, int events);
int e_iowatcher_poll_events (e_loop_t* loop, int timeout);

#endif