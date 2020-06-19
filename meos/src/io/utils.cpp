#include <meos/io/utils.hpp>
#include <meos/util/string.hpp>
#include <sstream>
#include <stdexcept>

void validate_ISO8601(const std::string &s) {
  if ((s[4] != '-') || (s[7] != '-')) {
    throw std::invalid_argument("Expected date in YYYY-MM-DD format");
  }

  // TODO check 1<= month <= 12
  // TODO check date according to year and month

  if ((s[10] != ' ') && (s[10] != 'T')) {
    throw std::invalid_argument("Expected either a space or a 'T' after day");
  }

  if ((s[13] != ':')) {
    throw std::invalid_argument("Expected time in hh:mm format");
  }

  if ((s[16] != ':')) {
    throw std::invalid_argument("Expected time in hh:mm:ss format");
  }

  if ((s[19] != '+') && (s[19] != '-')) {
    throw std::invalid_argument("Expected either a '+' or a '-' after time");
  }
}

std::string normalized_ISO8601(std::string s) {
  auto length = s.length();

  if (length < 10 || length > 24) {
    throw std::invalid_argument(
        "Empty or unexpected length for the provided ISO 8601 "
        "date/time string");
  }

  if (length == 10) { // 1234-12-12
    s += " 00:00:00+0000";
  } else if (length == 16) { // 1234-12-12T12:12
    s += ":00+0000";
  } else if (length == 19) { // 1234-12-12T12:12:12
    s += "+0000";
  } else if (length == 20) { // 1234-12-12T12:12:12Z
    if (s[19] != 'Z') {
      throw std::invalid_argument("For a ISO8601 string of length 20, "
                                  "expected 'Z' as the last character");
    }
    s[19] = '+';
    s += "0000";
  } else if (length == 22) { // 1234-12-12T12:12:12+05
    s += "00";
  }

  validate_ISO8601(s);

  return s;
}

time_t nextTime(std::istream &in) {
  // TODO allow strings like 2012-1-1 instead of just 2012-01-01

  in >> std::ws;
  std::string s = read_until_one_of(in, ",)]}\n");
  s = trim(s);
  int length = s.length();

  if (length != 24) {
    // We assume normalized patterns going forward (length 24)
    // If not already normalized, we normalize first and then parse
    s = normalized_ISO8601(s);
  }

  validate_ISO8601(s);
  std::stringstream ss(s);
  int temp_int;

  tm time = {0};
  ss >> time.tm_year;
  consume_one_of(ss, "-");
  ss >> time.tm_mon;
  consume_one_of(ss, "-");
  ss >> time.tm_mday;

  time.tm_year -= 1900;
  time.tm_mon -= 1;

  ss.get(); // skip the character
  ss >> time.tm_hour;
  consume_one_of(ss, ":");
  ss >> time.tm_min;
  consume_one_of(ss, ":");
  ss >> time.tm_sec;

  time.tm_isdst = 0;
  int millis = 0;
  // int millis = input.length() > 20 ? nextInt() : 0;

  int sign = (consume_one_of(ss, "+-") == '+') ? 1 : -1;
  int offset;
  ss >> offset;
  int h_offset = offset / 100;
  int m_offset = offset % 100;
  int tz_offset_secs = sign * (h_offset * 60 + m_offset) * 60;

  return (timegm(&time) - tz_offset_secs) * 1000 + millis;
}

char consume_one_of(std::istream &in, std::string s) {
  char c;
  if (s.find(c = in.get()) == std::string::npos) {
    throw std::invalid_argument("Expected one of '" + s + "'");
  }
  return c;
}

std::string read_until_one_of(std::istream &in, std::string stop_set) {
  char c;
  std::string s;
  while (c = in.peek()) {
    if ((c == EOF) || (stop_set.find(c) != std::string::npos)) {
      break;
    }
    s += in.get();
  }
  return s;
}
