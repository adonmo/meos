#ifndef MEOS_IO_UTILS_HPP
#define MEOS_IO_UTILS_HPP

#include <ctime>
#include <string>

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
time_t nextTime(std::istream &in);

char consume_one_of(std::istream &in, std::string s);

std::string read_until_one_of(std::istream &in, std::string end);

#endif
