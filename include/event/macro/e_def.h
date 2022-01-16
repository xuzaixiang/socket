//
// Created by 20123460 on 2022/1/13.
//

#ifndef EVENT_DEF_H
#define EVENT_DEF_H

#if defined(__cplusplus)
#define DEFAULT(x)  = x
#else
#define DEFAULT(x)
#endif

#if defined(__cplusplus)
#define BEGIN_EXTERN_C extern "C" {
#define END_EXTERN_C }
#else
#define BEGIN_EXTERN_C
#define END_EXTERN_C
#endif


#endif //EVENT_DEF_H
