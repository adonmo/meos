#ifndef MEOS_TESTS_COMMON_TIME_UTILS_HPP
#define MEOS_TESTS_COMMON_TIME_UTILS_HPP

#include <ctime>

const int seconds_per_minute = 60;
const int seconds_per_hour = 3600;
const int seconds_per_day = 86400;
const int days_of_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

bool is_leap_year(short year);

time_t unix_time(short year = 1970, short month = 1, short day = 1,
                 short hour = 0, short minute = 0, short second = 0);

#endif
