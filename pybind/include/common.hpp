#pragma once

#include <sstream>
#include <string>

template <typename T> std::string to_ostream(T self) {
  std::ostringstream s;
  s << self;
  return s.str();
}
