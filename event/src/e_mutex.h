//
// Created by 20123460 on 2022/2/9.
//

#ifndef EVENT_MUTEX_H
#define EVENT_MUTEX_H

#include <event/e_loop.h>

#ifdef EVENT_OS_WIN
#define e_mutex_t CRITICAL_SECTION
#define e_mutex_init InitializeCriticalSection
#define e_mutex_destroy DeleteCriticalSection
#define e_mutex_lock EnterCriticalSection
#define e_mutex_unlock LeaveCriticalSection
#else
#define e_mutex_t pthread_mutex_t
#define e_mutex_init(pmutex) pthread_mutex_init(pmutex, NULL)
#define e_mutex_destroy pthread_mutex_destroy
#define e_mutex_lock pthread_mutex_lock
#define e_mutex_unlock pthread_mutex_unlock

#define e_cond_t pthread_cond_t
#define e_cond_init(cond) pthread_cond_init(cond, NULL)
#define e_cond_wait(cond, mutex) pthread_cond_wait(cond, mutex)
#define e_cond_signal(cond) pthread_cond_signal(cond)
#define e_cond_broadcast(cond) pthread_cond_broadcast(cond)
#define e_cond_destroy(cond) pthread_cond_destroy(cond)
#endif

#endif // EVENT_MUTEX_H
