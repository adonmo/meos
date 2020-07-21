#include <algorithm>
#include <meos/io/utils.hpp>
#include <meos/types/geom/Geometry.hpp>
#include <meos/util/string.hpp>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace std;

template <typename T> T nextValue(istream &) {
  // Check specialized template functions below for supported types
  throw std::invalid_argument("Unsupported type");
}

template <> bool nextValue(istream &in) {
  in >> std::ws;
  string s = read_until_one_of(in, " @\n");
  transform(s.begin(), s.end(), s.begin(), ::tolower);

  bool value;
  if (s == "t" || s == "true") {
    value = true;
  } else if (s == "f" || s == "false") {
    value = false;
  } else {
    throw std::invalid_argument("Boolean value can only be one of (t, f, true, false), but got: "
                                + s);
  }

  return value;
}

template <> int nextValue(istream &in) {
  int value;
  in >> value;
  return value;
}

template <> float nextValue(istream &in) {
  float value;
  in >> value;
  return value;
}

template <> string nextValue(istream &in) {
  in >> std::ws;
  string input = read_until_one_of(in, "@");
  int length = input.length();

  // Skip double quotes if present
  if (length >= 2 && input[0] == '"' && input[input.length() - 1] == '"') {
    input = input.substr(1, length - 2);
  }

  if (length <= 0) {
    throw std::invalid_argument("Could not parse text: empty, unquoted value");
  }

  return input;
}

template <> Geometry nextValue(istream &in) {
  in >> std::ws;
  string input = read_until_one_of(in, "@");

  Geometry value(input);

  if (value.geom == nullptr) {
    throw std::invalid_argument("Could not parse geometry");
  }

  return value;
}

void validate_normalized_ISO8601(const string &s) {
  if (s.size() != 28) {
    throw invalid_argument(
        "Expected in YYYY-MM-DDThh:mm:ss.uuu+ZZZZ format "
        "(28 characters), got: "
        + s);
  }

  if ((s[4] != '-') || (s[7] != '-')) {
    throw invalid_argument("Expected date in YYYY-MM-DD format, got: " + s);
  }

  // TODO check 1<= month <= 12
  // TODO check date according to year and month

  if ((s[10] != ' ') && (s[10] != 'T')) {
    throw invalid_argument("Expected either a space or a 'T' after day, got: '" + string(1, s[10])
                           + "'");
  }

  if (s[13] != ':') {
    throw invalid_argument("Expected time in hh:mm:ss format, got: " + s);
  }

  if (s[16] != ':') {
    throw invalid_argument("Expected time in hh:mm:ss format, got: " + s);
  }

  if (s[19] != '.') {
    throw invalid_argument("Expected milliseconds after time, got: " + s);
  }

  // Milliseconds should be three characters
  if ((s[23] != '+') && (s[23] != '-')) {
    throw invalid_argument("Expected either a '+' or a '-' after time, got: '" + string(1, s[23])
                           + "'");
  }
}

string normalized_ISO8601(string s) {
  auto length = s.length();

  if (length < 10 || length > 28) {
    throw invalid_argument(
        "Empty or unexpected length for the provided ISO 8601 "
        "date/time string: "
        + s);
  }

  if (length == 10) {  // 1234-12-12
    s += " 00:00:00.000+0000";
  } else if (length == 16) {  // 1234-12-12T12:12
    s += ":00.000+0000";
  } else if (length == 19) {  // 1234-12-12T12:12:12
    s += ".000+0000";
  } else if (length == 20) {  // 1234-12-12T12:12:12Z
    if (s[19] != 'Z') {
      throw invalid_argument(
          "For a ISO8601 string of length 20, "
          "expected 'Z' as the last character, got: '"
          + string(1, s[19]) + "'");
    }
    s[19] = '.';
    s += "000+0000";
  } else if (s[19] == '.') {  // 1234-12-12T12:12:12.[...]
    // Milliseconds present - one character at min, three characters at max
    size_t tz_pos = s.find_first_of("+-Z", 21, 3);
    if (21 <= tz_pos && tz_pos <= 23) {
      // TZ also present
      string zero_padded_ms = string('0', 23 - tz_pos);
      string tz = s.substr(20);
      if (tz.length() == 2) {
        tz += "00";
      } else if (tz.length() != 4) {
        throw invalid_argument(
            "Unexpected tz offset: has to be either two or "
            "four characters long. Ex: +05, +0530, got: "
            + s);
      }
      s = s.substr(0, 19) + zero_padded_ms + "+" + tz;
    } else if (tz_pos == string::npos) {
      // TZ not present
      string zero_padded_ms = string('0', 23 - s.size());
      s = s.substr(0, 19) + zero_padded_ms + "+0000";
    } else {
      // TZ also (possibly) present, but beyond three characters
      throw invalid_argument(
          "Unexpected pattern, milliseconds can have a min "
          "of 1 characters and max of 3 characters, got: "
          + s);
    }
  } else {
    // Milliseconds not present
    if (length == 22) {  // 1234-12-12T12:12:12+05
      s = s.substr(0, 19) + ".000" + s.substr(19) + "00";
    } else if (length == 24) {  // 1234-12-12T12:12:12+0530
      s = s.substr(0, 19) + ".000" + s.substr(19);
    } else {
      throw invalid_argument("Unexpected pattern, got: " + s);
    }
  }

  validate_normalized_ISO8601(s);

  return s;
}

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
 * 1234-12-12T12:12:12.000
 * 1234-12-12 12:12:12.000Z
 * 1234-12-12 12:12:12.000+05
 * 1234-12-12 12:12:12.000+05
 * 1234-12-12 12:12:12.000-0530  // normalized pattern
 */
time_point nextTime(istream &in) {
  // TODO allow strings like 2012-1-1 instead of just 2012-01-01

  in >> std::ws;
  string s = read_until_one_of(in, ",)]}\n");
  s = trim(s);
  int length = s.length();

  if (length != 28) {
    // We assume normalized patterns going forward (length 24)
    // If not already normalized, we normalize first and then parse
    s = normalized_ISO8601(s);
  }

  validate_normalized_ISO8601(s);
  stringstream ss(s);

  tm time = {};
  ss >> time.tm_year;
  consume_one_of(ss, "-");
  ss >> time.tm_mon;
  consume_one_of(ss, "-");
  ss >> time.tm_mday;

  time.tm_year -= 1900;
  time.tm_mon -= 1;

  ss.get();  // skip the character
  ss >> time.tm_hour;
  consume_one_of(ss, ":");
  ss >> time.tm_min;
  consume_one_of(ss, ":");
  ss >> time.tm_sec;

  time.tm_isdst = 0;
  int millis = 0;
  if (ss.peek() == '.') {
    consume(ss, '.');
    ss >> millis;
  }

  int sign = (consume_one_of(ss, "+-") == '+') ? 1 : -1;
  int offset;
  ss >> offset;
  int h_offset = offset / 100;
  int m_offset = offset % 100;
  int tz_offset_secs = sign * (h_offset * 60 + m_offset) * 60;

  duration_ms duration((timegm(&time) - tz_offset_secs) * 1000L + millis);
  return time_point(duration);
}

void consume(istream &in, char expectedCharacter, bool skip_ws) {
  if (skip_ws) in >> std::ws;
  char c = in.get();
  if (c != expectedCharacter)
    throw invalid_argument("Expected '" + string(1, expectedCharacter) + "', got '" + c + "'");
}

void consume(istream &in, string expectedString, bool skip_ws) {
  if (skip_ws) in >> std::ws;
  for (auto c : expectedString)
    if (c != in.get()) throw invalid_argument("Expected '" + expectedString + "'");
}

char consume_one_of(istream &in, string charSet, bool skip_ws) {
  if (skip_ws) in >> std::ws;
  char c;
  if (charSet.find(c = in.get()) == string::npos) {
    throw invalid_argument("Expected one of '" + charSet + "', got: '" + string(1, c) + "'");
  }
  return c;
}

string read_until_one_of(istream &in, string stop_set) {
  char c;
  string s;
  while ((c = in.peek())) {
    if ((c == EOF) || (stop_set.find(c) != string::npos)) {
      break;
    }
    s += in.get();
  }
  return s;
}
