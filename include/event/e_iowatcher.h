#ifndef EVENT_IOWATCHER_H
#define EVENT_IOWATCHER_H

typedef struct e_loop_s e_loop_t;

int iowatcher_init        (e_loop_t* loop);
int iowatcher_cleanup     (e_loop_t* loop);
int iowatcher_add_event   (e_loop_t* loop, int fd, int events);
int iowatcher_del_event   (e_loop_t* loop, int fd, int events);
int iowatcher_poll_events (e_loop_t* loop, int timeout);

#endif