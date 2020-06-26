#include <meos/util/string.hpp>

std::string ws(" \t\n\r\f\v");

// trim from end of string (right)
std::string &rtrim(std::string &s, std::string const &t) {
  s.erase(s.find_last_not_of(t) + 1);
  return s;
}

// trim from beginning of string (left)
std::string &ltrim(std::string &s, std::string const &t) {
  s.erase(0, s.find_first_not_of(t));
  return s;
}

// trim from both ends of string (right then left)
std::string &trim(std::string &s, std::string const &t) {
  return ltrim(rtrim(s, t), t);
}
