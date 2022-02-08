//
// Created by 20123460 on 2022/1/17.
//

#ifndef EVENT_MATH_H
#define EVENT_MATH_H

unsigned long ceil2e(unsigned long num);
long long varint_decode(const unsigned char* buf, int* len);

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef LIMIT
#define LIMIT(lower, v, upper) ((v) < (lower) ? (lower) : (v) > (upper) ? (upper) : (v))
#endif

#endif //EVENT_MATH_H
