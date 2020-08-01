#pragma once

#include <chrono>
#include <string>

namespace meos {

using time_point = std::chrono::system_clock::time_point;

std::string write_ISO8601_time(time_point const &t);

}  // namespace meos
