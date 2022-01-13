//
// Created by 20123460 on 2022/1/13.
//

#ifndef EVENT_EXPORT_H
#define EVENT_EXPORT_H

#if defined(_MSC_VER)
#define EVENT_EXPORT __declspec(dllexport)
#elif defined(__GNUC__)
#define EVENT_EXPORT __attribute__((visibility("default")))
#else
#define EVENT_EXPORT
#endif


#endif //EVENT_EXPORT_H
