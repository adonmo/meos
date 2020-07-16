#ifndef MEOS_TESTS_COMMON_TIME_UTILS_HPP
#define MEOS_TESTS_COMMON_TIME_UTILS_HPP

#include <chrono>
#include <ctime>

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

int const seconds_per_minute = 60;
int const seconds_per_hour = 3600;
int const seconds_per_day = 86400;
int const days_of_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

bool is_leap_year(short year);

time_t unix_time(short year = 1970, short month = 1, short day = 1,
                 short hour = 0, short minute = 0, short second = 0,
                 short milliseconds = 0);

time_point unix_time_point(short year = 1970, short month = 1, short day = 1,
                           short hour = 0, short minute = 0, short second = 0,
                           short milliseconds = 0);

#endif
