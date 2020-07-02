#ifndef MEOS_IO_UTILS_HPP
#define MEOS_IO_UTILS_HPP

#include <chrono>
#include <string>

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

template <typename T> T nextValue(std::istream &in);

void validate_ISO8601(const std::string &s);

std::string normalized_ISO8601(std::string s);

/**
 * Parse time in ISO8601 format
 * Skips initial whitespaces, reads until one of ",)]}\n" is reached, and tries
 * to parse everything in between as time in ISO8601 format
 *
 * Example patterns:
 * 1234-12-12
 * 1234-12-12 12:12
 * 1234-12-12T12:12
 * 1234-12-12 12:12:12
 * 1234-12-12T12:12:12
 * 1234-12-12 12:12:12Z
 * 1234-12-12 12:12:12+05
 * 1234-12-12 12:12:12-0530  // normalized pattern
 */
time_point nextTime(std::istream &in);

void consume(std::istream &in, char expectedCharacter, bool skip_ws = true);
void consume(std::istream &in, std::string expectedString, bool skip_ws = true);
char consume_one_of(std::istream &in, std::string charSet, bool skip_ws = true);

std::string read_until_one_of(std::istream &in, std::string end);

#endif
