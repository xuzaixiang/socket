//
// Created by 20123460 on 2022/2/8.
//

#ifndef EVENT_DEF_H
#define EVENT_DEF_H

#ifdef __cplusplus
#define BEGIN_EXTERN_C extern "C" {
#define END_EXTERN_C } // extern "C"
#else
#define BEGIN_EXTERN_C
#define END_EXTERN_C
#endif

#if defined(_MSC_VER)
#if defined(EVENT_DYNAMIC)
#define EVENT_EXPORT __declspec(dllexport)
#else
#define EVENT_EXPORT __declspec(dllimport)
#endif
#elif defined(__GNUC__)
#define EVENT_EXPORT __attribute__((visibility("default")))
#else
#define EVENT_EXPORT
#endif



#endif // EVENT_DEF_H