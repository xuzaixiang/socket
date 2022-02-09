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
#endif

#endif // EVENT_MUTEX_H
