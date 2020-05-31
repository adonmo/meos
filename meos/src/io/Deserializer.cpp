#include <algorithm>
#include <ctime>
#include <meos/io/DeserializationException.hpp>
#include <meos/io/Deserializer.hpp>
#include <sstream>
#include <vector>
using namespace std;

template <typename T>
Deserializer<T>::Deserializer(const string &in_) : in(in_) {
  iter = in.begin();
}

template <typename T> unique_ptr<Temporal<T>> Deserializer<T>::nextTemporal() {
  skipWhitespaces();
  const char lookahead1 = peek(0);
  const char lookahead2 = peek(1);
  if (lookahead1 != '{' && lookahead1 != '[' && lookahead1 != '(') {
    return nextTInstant();
  } else if (lookahead1 == '[' || lookahead1 == '(') {
    return nextTSequence();
  } else if (lookahead1 == '{') {
    if (lookahead2 == '[' || lookahead2 == '(') {
      throw new DeserializationException("TSequenceSet not implemented yet");
      // return TSequenceSet<T>(value);
    } else {
      return nextTInstantSet();
    }
  }
  throw new DeserializationException("Invalid Temporal");
};

template <typename T>
unique_ptr<TSequence<T>> Deserializer<T>::nextTSequence() {
  skipWhitespaces();

  if (!hasNext() || !(peek(0) == '[' || peek(0) == '(')) {
    throw DeserializationException("Expected either a '[' or '('");
  }

  const char opening = peek(0);
  consumeChar(opening);
  const bool left_open = opening == '(';

  vector<unique_ptr<TInstant<T>>> v = {};
  v.push_back(nextTInstant());
  skipWhitespaces();

  while (hasNext() && peek(0) == ',') {
    consumeChar(',');
    v.push_back(nextTInstant());
    skipWhitespaces();
  }

  if (!hasNext() || !(peek(0) == ']' || peek(0) == ')')) {
    throw DeserializationException("Expected either a ']' or ')'");
  }

  const char closing = peek(0);
  consumeChar(closing);
  const bool right_open = closing == ')';

  return make_unique<TSequence<T>>(v, left_open, right_open);
};

template <typename T>
unique_ptr<TInstantSet<T>> Deserializer<T>::nextTInstantSet() {
  skipWhitespaces();
  consumeChar('{');
  set<unique_ptr<TInstant<T>>> s = {};
  s.insert(nextTInstant());
  skipWhitespaces();

  while (hasNext() && peek(0) == ',') {
    consumeChar(',');
    s.insert(nextTInstant());
    skipWhitespaces();
  }

  if (!hasNext() || peek(0) != '}') {
    throw DeserializationException("Expected a '}'");
  }
  consumeChar('}');

  return make_unique<TInstantSet<T>>(s);
};

template <typename T> unique_ptr<TInstant<T>> Deserializer<T>::nextTInstant() {
  skipWhitespaces();
  T value = nextValue();
  string::size_type pos = in.find_first_of("@", iter - in.begin());
  if (pos == string::npos) {
    throw DeserializationException("Invalid TInstant: needs to contain @");
  }
  consumeChar('@');
  time_t t = nextTime();

  return make_unique<TInstant<T>>(value, t);
};

template <typename T> unique_ptr<Period> Deserializer<T>::nextPeriod() {
  skipWhitespaces();

  if (!hasNext() || !(peek(0) == '[' || peek(0) == '(')) {
    throw DeserializationException("Expected either a '[' or '('");
  }

  const char opening = peek(0);
  consumeChar(opening);
  const bool left_open = opening == '(';

  auto lbound = nextTime();
  skipWhitespaces();
  skipWhitespaces();
  consumeChar(',');
  auto rbound = nextTime();

  if (!hasNext() || !(peek(0) == ']' || peek(0) == ')')) {
    throw DeserializationException("Expected either a ']' or ')'");
  }

  const char closing = peek(0);
  consumeChar(closing);
  const bool right_open = closing == ')';

  return make_unique<Period>(lbound, rbound, left_open, right_open);
};

template <typename T> unique_ptr<PeriodSet> Deserializer<T>::nextPeriodSet() {
  skipWhitespaces();
  consumeChar('{');
  set<unique_ptr<Period>> s = {};
  s.insert(nextPeriod());
  skipWhitespaces();

  while (hasNext() && peek(0) == ',') {
    consumeChar(',');
    s.insert(nextPeriod());
    skipWhitespaces();
  }

  if (!hasNext() || peek(0) != '}') {
    throw DeserializationException("Expected a '}'");
  }
  consumeChar('}');

  return make_unique<PeriodSet>(s);
};

/**
 * Parse time in ISO8601 format
 * Skips initial whitespaces, reads until one of ",)]}\n" is reached, and tries
 * to parse everything in between as time in ISO8601 format
 */
template <typename T> time_t Deserializer<T>::nextTime() {
  // TODO add support for UTC offset/timezone
  // TODO allow strings like 2012-1-1 instead of just 2012-01-01

  skipWhitespaces();
  string::size_type current_pos = iter - in.begin();
  string::size_type end_pos = in.find_first_of(",)]}\n", current_pos);
  if (end_pos == string::npos) {
    end_pos = in.end() - in.begin();
  }
  int length = end_pos - current_pos;
  string input = in.substr(current_pos, length);
  if (length == 10) {
    // 1234-12-12
    input += "T00:00:00Z";
  } else if (length == 16) {
    // 1234-12-12T12:12
    input += ":00Z";
  } else if (length == 19) {
    // 1234-12-12T12:12:12
    input += "Z";
  }

  constexpr const size_t expectedLength = sizeof("1234-12-12T12:12:12Z") - 1;
  static_assert(expectedLength == 20, "Unexpected ISO 8601 date/time length");

  if (input.length() < expectedLength) {
    throw DeserializationException(
        "Empty or unexpected length for the provided ISO 8601 "
        "date/time string");
  }

  tm time = {0};
  time.tm_year = nextInt() - 1900;
  consumeChar('-');
  time.tm_mon = nextInt() - 1;
  consumeChar('-');
  time.tm_mday = nextInt();

  int millis = 0;

  if (iter - in.begin() != end_pos) {
    if (*iter != ' ' && (*iter != 'T')) {
      throw DeserializationException(
          "Expected either a space or a 'T' after day");
    }
    consumeChar(*iter); // skip the character
    time.tm_hour = nextInt();
    consumeChar(':');
    time.tm_min = nextInt();
    consumeChar(':');
    time.tm_sec = nextInt();
    time.tm_isdst = 0;
    millis = input.length() > 20 ? nextInt() : 0;
  }

  return timegm(&time) * 1000 + millis;
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
  } catch (invalid_argument e) {
    throw DeserializationException("Could not parse: invalid argument");
  } catch (out_of_range e) {
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

template <typename T> void Deserializer<T>::consumeChar(const char c) {
  string::size_type current_pos = iter - in.begin();
  if (*iter != c) {
    std::stringstream error;
    error << "Expected character '" << c << "' at position " << current_pos;
    throw DeserializationException(error.str());
  }
  iter += 1;
}

template <typename T> void Deserializer<T>::skipChars(const string &chars) {
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
