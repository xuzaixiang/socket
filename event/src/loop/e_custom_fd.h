//
// Created by 20123460 on 2022/2/9.
//

#ifndef EVENT_CUSTOM_FD_H
#define EVENT_CUSTOM_FD_H

typedef struct e_loop_s e_loop_t;

void e_loop_reset_custom_fd(e_loop_t *loop);
void e_loop_close_custom_fd(e_loop_t *loop);
int e_loop_create_custom_fd(e_loop_t *loop);
int e_loop_get_custom_write_fd(e_loop_t *loop);
int e_loop_get_custom_read_fd(e_loop_t *loop);
void e_loop_handle_custom_event(e_io_t* io);
#endif // EVENT_CUSTOM_FD_H
