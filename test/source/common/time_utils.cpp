#include "time_utils.hpp"

time_t unix_time(short year, short month, short day, short hour, short minute,
                 short second, short milliseconds) {
  struct std::tm epoch;
  epoch.tm_isdst = 0;
  epoch.tm_sec = epoch.tm_min = epoch.tm_hour = epoch.tm_mon = 0;
  epoch.tm_mday = 1;
  epoch.tm_year = 70;
  time_t local_tz_offset = mktime(&epoch) * 1000L;

  struct tm timeinfo = {};
  timeinfo.tm_year = year - 1900;
  timeinfo.tm_mon = month - 1;
  timeinfo.tm_mday = day;
  timeinfo.tm_hour = hour;
  timeinfo.tm_min = minute;
  timeinfo.tm_sec = second;
  return mktime(&timeinfo) * 1000L - local_tz_offset + milliseconds;
}

time_point unix_time_point(short year, short month, short day, short hour,
                           short minute, short second, short milliseconds) {
  return time_point(duration_ms(
      unix_time(year, month, day, hour, minute, second, milliseconds)));
}
