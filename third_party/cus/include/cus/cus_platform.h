#ifndef CUS_PLATFORM_H
#define CUS_PLATFORM_H

#if defined(WIN64) || defined(_WIN64)
#define CUS_OS_WIN64
#define CUS_OS_WIN32
#elif defined(WIN32) || defined(_WIN32)
#define CUS_OS_WIN32
#elif defined(ANDROID) || defined(__ANDROID__)
#define CUS_OS_ANDROID
#define CUS_OS_LINUX
#elif defined(linux) || defined(__linux) || defined(__linux__)
#define CUS_OS_LINUX
#elif defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__) || defined(__xlc__))
#include <TargetConditionals.h>
#if defined(TARGET_OS_MAC) && TARGET_OS_MAC
#define CUS_OS_MAC
#elif defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#define CUS_OS_IOS
#endif
#define CUS_OS_DARWIN
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#define CUS_OS_FREEBSD
#define CUS_OS_BSD
#elif defined(__NetBSD__)
#define CUS_OS_NETBSD
#define CUS_OS_BSD
#elif defined(__OpenBSD__)
#define CUS_OS_OPENBSD
#define CUS_OS_BSD
#elif defined(sun) || defined(__sun) || defined(__sun__)
#define CUS_OS_SOLARIS
#else
#error "unsupport operating system platform!"
#endif

#if defined(CUS_OS_WIN32) || defined(CUS_OS_WIN64)
#undef  CUS_OS_UNIX
#define CUS_OS_WIN
#else
#define CUS_OS_UNIX
#endif

#endif //CUS_PLATFORM_H