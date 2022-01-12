

#include "e_time.h"

unsigned long long gethrtime_us() {
#ifdef OS_WIN
  static LONGLONG s_freq = 0;
    if (s_freq == 0) {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        s_freq = freq.QuadPart;
    }
    if (s_freq != 0) {
        LARGE_INTEGER count;
        QueryPerformanceCounter(&count);
        return (unsigned long long)(count.QuadPart / (double)s_freq * 1000000);
    }
    return 0;
#elif HAVE_CLOCK_GETTIME
  struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec*(unsigned long long)1000000 + ts.tv_nsec / 1000;
#else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec*(unsigned long long)1000000 + tv.tv_usec;
#endif
}

unsigned long long gettimeofday_ms(){
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * (unsigned long long)1000 + tv.tv_usec/1000;
}
unsigned long long gettimeofday_us(){
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * (unsigned long long)1000000 + tv.tv_usec;
}

time_t cron_next_timeout(int minute, int hour, int day, int week, int month){
  enum {
    MINUTELY,
    HOURLY,
    DAILY,
    WEEKLY,
    MONTHLY,
    YEARLY,
  } period_type = MINUTELY;
  struct tm tm;
  time_t tt;
  time(&tt);
  tm = *localtime(&tt);
  time_t tt_round = 0;

  tm.tm_sec = 0;
  if (minute >= 0) {
    period_type = HOURLY;
    tm.tm_min = minute;
  }
  if (hour >= 0) {
    period_type = DAILY;
    tm.tm_hour = hour;
  }
  if (week >= 0) {
    period_type = WEEKLY;
  }
  else if (day > 0) {
    period_type = MONTHLY;
    tm.tm_mday = day;
    if (month > 0) {
      period_type = YEARLY;
      tm.tm_mon = month - 1;
    }
  }

  tt_round = mktime(&tm);
  if (week >= 0) {
    tt_round = tt + (week-tm.tm_wday)*SECONDS_PER_DAY;
  }
  if (tt_round > tt) {
    return tt_round;
  }

  switch(period_type) {
    case MINUTELY:
      tt_round += SECONDS_PER_MINUTE;
      return tt_round;
    case HOURLY:
      tt_round += SECONDS_PER_HOUR;
      return tt_round;
    case DAILY:
      tt_round += SECONDS_PER_DAY;
      return tt_round;
    case WEEKLY:
      tt_round += SECONDS_PER_WEEK;
      return tt_round;
    case MONTHLY:
      if (++tm.tm_mon == 12) {
        tm.tm_mon = 0;
        ++tm.tm_year;
      }
      break;
    case YEARLY:
      ++tm.tm_year;
      break;
    default:
      return -1;
  }

  return mktime(&tm);
}