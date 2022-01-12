#ifndef EVENT_PTHREAD_H
#define EVENT_PTHREAD_H

#include <pthread.h>

#define e_mutex_t                 pthread_mutex_t
#define e_mutex_init(e_mutex)     pthread_mutex_init(e_mutex, NULL)
#define e_mutex_destroy           pthread_mutex_destroy
#define e_mutex_lock              pthread_mutex_lock
#define e_mutex_unlock            pthread_mutex_unlock

#define e_recursive_mutex_t          pthread_mutex_t
#define e_recursive_mutex_init(e_mutex) \
    do {\
        pthread_mutexattr_t attr;\
        pthread_mutexattr_init(&attr);\
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);\
        pthread_mutex_init(e_mutex, &attr);\
    } while(0)
#define e_recursive_mutex_destroy    pthread_mutex_destroy
#define e_recursive_mutex_lock       pthread_mutex_lock
#define e_recursive_mutex_unlock     pthread_mutex_unlock

#endif //EVENT_PTHREAD_H