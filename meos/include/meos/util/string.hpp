#ifndef MEOS_UTIL_STRING_HPP
#define MEOS_UTIL_STRING_HPP

#include <string>

extern std::string ws;

// trim from end of string (right)
std::string &rtrim(std::string &s, std::string const &t = ws);

// trim from beginning of string (left)
std::string &ltrim(std::string &s, std::string const &t = ws);

// trim from both ends of string (right then left)
std::string &trim(std::string &s, std::string const &t = ws);

#endif
