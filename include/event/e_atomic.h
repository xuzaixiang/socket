#ifndef EVENT_ATOMIC_H
#define EVENT_ATOMIC_H

#if defined(__cplusplus) && __cplusplus >= 201103L
#include <atomic>
using std::atomic_flag;
using std::atomic_long;
#define ATOMIC_FLAG_TEST_AND_SET(p)     ((p)->test_and_set())
#define ATOMIC_FLAG_CLEAR(p)            ((p)->clear())
#define ATOMIC_ADD(p, n)                (*(p) += (n))
#define ATOMIC_SUB(p, n)                (*(p) -= (n))
#define ATOMIC_INC(p)                   ((*(p))++)
#define ATOMIC_DEC(p)                   ((*(p))--)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#include <stdatomic.h>
#define ATOMIC_FLAG_TEST_AND_SET    atomic_flag_test_and_set
#define ATOMIC_FLAG_CLEAR           atomic_flag_clear
#define ATOMIC_ADD                  atomic_fetch_add
#define ATOMIC_SUB                  atomic_fetch_sub
#define ATOMIC_INC(p)               ATOMIC_ADD(p, 1)
#define ATOMIC_DEC(p)               ATOMIC_SUB(p, 1)
#else
#error "must be c11 or cpp11"
#endif

typedef atomic_flag e_atomic_flag_t;
#define EVENT_ATOMIC_VAR_INIT           ATOMIC_FLAG_INIT
#define e_atomic_flag_test_and_set   ATOMIC_FLAG_TEST_AND_SET
#define e_atomic_flag_clear          ATOMIC_FLAG_CLEAR

typedef atomic_long e_atomic_t;
#define EVENT_ATOMIC_VAR_INIT            ATOMIC_VAR_INIT
#define e_atomic_add                 ATOMIC_ADD
#define e_atomic_sub                 ATOMIC_SUB
#define e_atomic_inc                 ATOMIC_INC
#define e_atomic_dec                 ATOMIC_DEC

#endif //EVENT_ATOMIC_H