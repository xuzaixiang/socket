//
// Created by 20123460 on 2022/2/9.
//
#include <event/e_loop.h>
#include <pthread.h>
#include <unistd.h>

void *test(void *loop) {
  e_loop_run(((e_loop_t *)loop));
  return NULL;
}
int main() {
  e_loop_t *loop = e_loop_new(0);
  pthread_t thread;
  pthread_create(&thread, NULL, test, loop);

  sleep(5);
  e_loop_pause(loop);
  pthread_join(thread, NULL);

  e_loop_free(&loop);
  return 0;
}