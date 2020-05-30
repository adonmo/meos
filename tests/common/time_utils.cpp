#include "time_utils.hpp"

time_t unix_time(short year, short month, short day, short hour, short minute,
                 short second) {
  struct std::tm epoch;
  epoch.tm_isdst = 0;
  epoch.tm_sec = epoch.tm_min = epoch.tm_hour = epoch.tm_mon = 0;
  epoch.tm_mday = 1;
  epoch.tm_year = 70;
  time_t local_tz_offset = mktime(&epoch) * 1000;

  struct tm timeinfo = {0};
  timeinfo.tm_year = year - 1900;
  timeinfo.tm_mon = month - 1;
  timeinfo.tm_mday = day;
  timeinfo.tm_hour = hour;
  timeinfo.tm_min = minute;
  timeinfo.tm_sec = second;
  return mktime(&timeinfo) * 1000 - local_tz_offset;
}
