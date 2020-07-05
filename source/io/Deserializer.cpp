#include <algorithm>
#include <ctime>
#include <meos/io/DeserializationException.hpp>
#include <meos/io/Deserializer.hpp>
#include <meos/io/utils.hpp>
#include <sstream>
using namespace std;

template <typename T>
Deserializer<T>::Deserializer(string const &in_) : in(in_) {
  iter = in.begin();
}

template <typename T> unique_ptr<Temporal<T>> Deserializer<T>::nextTemporal() {
  skipWhitespaces();
  char const lookahead1 = peek(0);
  char const lookahead2 = peek(1);
  if (lookahead1 != '{' && lookahead1 != '[' && lookahead1 != '(') {
    return nextTInstant();
  } else if (lookahead1 == '[' || lookahead1 == '(') {
    return nextTSequence();
  } else if (lookahead1 == '{') {
    if (lookahead2 == '[' || lookahead2 == '(') {
      return nextTSequenceSet();
    } else {
      return nextTInstantSet();
    }
  }
  throw new DeserializationException("Invalid Temporal");
}

template <typename T>
unique_ptr<TSequenceSet<T>> Deserializer<T>::nextTSequenceSet() {
  skipWhitespaces();
  consumeChar('{');
  set<TSequence<T>> s = {};
  s.insert(*nextTSequence().get());
  skipWhitespaces();

  while (hasNext() && peek(0) == ',') {
    consumeChar(',');
    s.insert(*nextTSequence().get());
    skipWhitespaces();
  }

  if (!hasNext() || peek(0) != '}') {
    throw DeserializationException("Expected a '}'");
  }
  consumeChar('}');

  return make_unique<TSequenceSet<T>>(s);
}

template <typename T>
unique_ptr<TSequence<T>> Deserializer<T>::nextTSequence() {
  skipWhitespaces();

  if (!hasNext() || !(peek(0) == '[' || peek(0) == '(')) {
    throw DeserializationException("Expected either a '[' or '('");
  }

  char const opening = peek(0);
  consumeChar(opening);
  bool const lower_inc = opening == '[';

  set<TInstant<T>> s = {};
  s.insert(*(nextTInstant().get()));
  skipWhitespaces();

  while (hasNext() && peek(0) == ',') {
    consumeChar(',');
    s.insert(*(nextTInstant().get()));
    skipWhitespaces();
  }

  if (!hasNext() || !(peek(0) == ']' || peek(0) == ')')) {
    throw DeserializationException("Expected either a ']' or ')'");
  }

  char const closing = peek(0);
  consumeChar(closing);
  bool const upper_inc = closing == ']';

  return make_unique<TSequence<T>>(s, lower_inc, upper_inc);
}

template <typename T>
unique_ptr<TInstantSet<T>> Deserializer<T>::nextTInstantSet() {
  skipWhitespaces();
  consumeChar('{');
  set<TInstant<T>> s = {};
  s.insert(*nextTInstant().get());
  skipWhitespaces();

  while (hasNext() && peek(0) == ',') {
    consumeChar(',');
    s.insert(*nextTInstant().get());
    skipWhitespaces();
  }

  if (!hasNext() || peek(0) != '}') {
    throw DeserializationException("Expected a '}'");
  }
  consumeChar('}');

  return make_unique<TInstantSet<T>>(s);
}

template <typename T> unique_ptr<TInstant<T>> Deserializer<T>::nextTInstant() {
  skipWhitespaces();
  T value = nextValue();
  string::size_type pos = in.find_first_of("@", iter - in.begin());
  if (pos == string::npos) {
    throw DeserializationException("Invalid TInstant: needs to contain @");
  }
  consumeChar('@');
  time_point t = nextTime();

  return make_unique<TInstant<T>>(value, t);
}

template <typename T> unique_ptr<Period> Deserializer<T>::nextPeriod() {
  // TODO refactor Deserializer to simplify this futher
  string::size_type current_pos = iter - in.begin();
  string s = in.substr(current_pos, 256);
  stringstream ss(s);
  Period period;
  ss >> period;
  iter += ss.tellg();
  return period.clone();
}

template <typename T> unique_ptr<PeriodSet> Deserializer<T>::nextPeriodSet() {
  string::size_type current_pos = iter - in.begin();
  string s = in.substr(current_pos, 2048);
  stringstream ss(s);
  PeriodSet period_set;
  ss >> period_set;
  iter += ss.tellg();
  return period_set.clone();
}

template <typename T>
unique_ptr<TimestampSet> Deserializer<T>::nextTimestampSet() {
  string::size_type current_pos = iter - in.begin();
  string s = in.substr(current_pos, 2048);
  stringstream ss(s);
  TimestampSet timestamp_set;
  ss >> timestamp_set;
  iter += ss.tellg();
  return make_unique<TimestampSet>(timestamp_set);
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
 * 1234-12-12 12:12:12Z
 * 1234-12-12 12:12:12+05
 * 1234-12-12 12:12:12-0530  // normalized pattern
 */
template <typename T> time_point Deserializer<T>::nextTime() {
  // TODO add support for UTC offset/timezone
  // TODO allow strings like 2012-1-1 instead of just 2012-01-01

  skipWhitespaces();
  string::size_type current_pos = iter - in.begin();
  string::size_type end_pos = in.find_first_of(",)]}\n", current_pos);
  if (end_pos == string::npos) {
    end_pos = in.end() - in.begin();
  }
  int length = end_pos - current_pos;

  if (length != 24) {
    // We only parse normalized patterns (length 24)
    // If not already normalized, we normalize first and then parse
    string input = normalized_ISO8601(in.substr(current_pos, length));
    Deserializer<T> mini_deserializer(input);
    iter += length;
    return mini_deserializer.nextTime();
  }

  validate_ISO8601(in.substr(current_pos, length));

  tm time = {};
  time.tm_year = nextInt() - 1900;
  consumeChar('-');
  time.tm_mon = nextInt() - 1;
  consumeChar('-');
  time.tm_mday = nextInt();

  consumeChar(*iter); // skip the character
  time.tm_hour = nextInt();
  consumeChar(':');
  time.tm_min = nextInt();
  consumeChar(':');
  time.tm_sec = nextInt();
  time.tm_isdst = 0;
  int millis = 0;
  // int millis = input.length() > 20 ? nextInt() : 0;

  int sign = *iter == '+' ? 1 : -1;
  consumeChar(*iter); // skip the character
  int offset = nextInt();
  int h_offset = offset / 100;
  int m_offset = offset % 100;
  int tz_offset_secs = sign * (h_offset * 60 + m_offset) * 60;

  duration_ms duration((timegm(&time) - tz_offset_secs) * 1000L + millis);
  return time_point(duration);
}

template <typename T> T Deserializer<T>::nextValue() {
  // Check specialized template functions below for supported types
  throw DeserializationException("Unsupported type");
}

template <> bool Deserializer<bool>::nextValue() {
  skipWhitespaces();
  string s = string(iter, in.end());
  string::size_type current_pos = iter - in.begin();
  string::size_type end_pos = in.find_first_of(" @\n", current_pos);
  if (end_pos == string::npos) {
    end_pos = in.end() - in.begin();
  }
  int length = end_pos - current_pos;
  string input = in.substr(current_pos, length);
  transform(input.begin(), input.end(), input.begin(), ::tolower);

  bool value;
  if (input == "t" || input == "true") {
    value = true;
  } else if (input == "f" || input == "false") {
    value = false;
  } else {
    throw DeserializationException(
        "Boolean value can only be one of (t, f, true, false), but got: " +
        input);
  }

  iter += length;
  return value;
}

template <> int Deserializer<int>::nextValue() {
  skipWhitespaces();
  string s = string(iter, in.end());
  string::size_type length;
  int value = stoi(s, &length, 10);
  iter += length;
  return value;
}

template <> float Deserializer<float>::nextValue() {
  skipWhitespaces();
  string s = string(iter, in.end());
  string::size_type length;
  float value = stof(s, &length);
  iter += length;
  return value;
}

template <> string Deserializer<string>::nextValue() {
  skipWhitespaces();
  string::size_type current_pos = iter - in.begin();
  string::size_type end_pos = in.find_first_of("@", current_pos);
  if (end_pos == string::npos) {
    end_pos = in.end() - in.begin();
  }
  int length = end_pos - current_pos;
  string input = in.substr(current_pos, length);

  // Skip double quotes if present
  if (length >= 2 && input[0] == '"' && input[input.length() - 1] == '"') {
    input = input.substr(1, length - 2);
  }

  if (length <= 0) {
    throw DeserializationException(
        "Could not parse text: empty, unquoted value");
  }

  iter += length;
  return input;
}

template <> Geometry Deserializer<Geometry>::nextValue() {
  skipWhitespaces();
  string::size_type current_pos = iter - in.begin();
  string::size_type end_pos = in.find_first_of("@", current_pos);
  if (end_pos == string::npos) {
    end_pos = in.end() - in.begin();
  }
  int length = end_pos - current_pos;
  string input = in.substr(current_pos, length);

  Geometry value(input);

  if (value.geom == nullptr) {
    throw DeserializationException("Could not parse geometry");
  }

  iter += length;
  return value;
}

template <typename T> int Deserializer<T>::nextInt() {
  skipWhitespaces();
  try {
    string s = string(iter, in.end());
    string::size_type length;
    int ivalue = stoi(s, &length, 10);
    iter += length;
    return ivalue;
  } catch (invalid_argument &) {
    throw DeserializationException("Could not parse: invalid argument");
  } catch (out_of_range &) {
    throw DeserializationException("Could not parse: out of range");
  }
}

template <typename T> char Deserializer<T>::peek(int lookahead) {
  if ((in.end() - iter) < lookahead) {
    throw DeserializationException("Reached end of stream");
  };
  return *(iter + lookahead);
}

template <typename T> void Deserializer<T>::skipWhitespaces() {
  skipChars(" \t\n");
}

template <typename T> void Deserializer<T>::consumeChar(char const c) {
  string::size_type current_pos = iter - in.begin();
  if (*iter != c) {
    std::stringstream error;
    error << "Expected character '" << c << "' at position " << current_pos;
    throw DeserializationException(error.str());
  }
  iter += 1;
}

template <typename T> void Deserializer<T>::skipChars(string const &chars) {
  string::size_type current_pos = iter - in.begin();
  string::size_type end_pos = in.find_first_not_of(chars, current_pos);
  if (end_pos == string::npos) {
    end_pos = in.end() - in.begin();
  }
  int length = end_pos - current_pos;
  iter += length;
}

template <typename T> bool Deserializer<T>::hasNext() {
  return iter != in.end();
}
