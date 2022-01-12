
#ifndef EVENT_TIME_H
#define EVENT_TIME_H

#include "event/e_api.h"

unsigned long long gethrtime_us();
unsigned long long gettimeofday_ms();
unsigned long long gettimeofday_us();

#define SECONDS_PER_MINUTE  60
#define SECONDS_PER_HOUR    3600
#define SECONDS_PER_DAY     86400   // 24*3600
#define SECONDS_PER_WEEK    604800  // 7*24*3600
/*
 * minute   hour    day     week    month       action
 * 0~59     0~23    1~31    0~6     1~12
 *  -1      -1      -1      -1      -1          cron.minutely
 *  30      -1      -1      -1      -1          cron.hourly
 *  30      1       -1      -1      -1          cron.daily
 *  30      1       15      -1      -1          cron.monthly
 *  30      1       -1       0      -1          cron.weekly
 *  30      1        1      -1      10          cron.yearly
 */
time_t cron_next_timeout(int minute, int hour, int day, int week, int month);
#endif