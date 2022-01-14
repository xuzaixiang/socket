#ifndef EVENT_PLATFORM_H
#define EVENT_PLATFORM_H

#if defined(WIN64) || defined(_WIN64)
#define EVENT_OS_WIN64
#define EVENT_OS_WIN32
#elif defined(WIN32) || defined(_WIN32)
#define EVENT_OS_WIN32
#elif defined(linux) || defined(__linux) || defined(__linux__)
#define EVENT_OS_LINUX
#elif defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__) || defined(__xlc__))
#include <TargetConditionals.h>
#if defined(TARGET_OS_MAC) && TARGET_OS_MAC
#define EVENT_OS_MAC
#else "unsupport operating system platform!"
#error
#endif
#else
#error "unsupport operating system platform!"
#endif

#if defined(EVENT_OS_WIN32) || defined(EVENT_OS_WIN64)
#define EVENT_OS_WIN
#endif

#endif //EVENT_PLATFORM_H