#ifndef MEOS_UTIL_TIME_HPP
#define MEOS_UTIL_TIME_HPP

#include <chrono>
#include <string>

using time_point = std::chrono::system_clock::time_point;

std::string write_ISO8601_time(time_point const &t);

template <typename T> std::string write_value(T const &t);

#endif
