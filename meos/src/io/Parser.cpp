#include <ctime>
#include <meos/io/ParseException.hpp>
#include <meos/io/Parser.hpp>
#include <sstream>
#include <vector>
using namespace std;

template <typename T> Parser<T>::Parser(const string &in_) : in(in_) {
  iter = in.begin();
}

template <typename T> unique_ptr<Temporal<T>> Parser<T>::parseNextTemporal() {
  skipWhitespaces();
  const char lookahead1 = peek(0);
  const char lookahead2 = peek(1);
  if (lookahead1 != '{' && lookahead1 != '[' && lookahead1 != '(') {
    return parseNextTInstant();
  } else if (lookahead1 == '[' || lookahead1 == '(') {
    return parseNextTSequence();
  } else if (lookahead1 == '{') {
    if (lookahead2 == '[' || lookahead2 == '(') {
      throw new ParseException("TSequenceSet not implemented yet");
      // return TSequenceSet<T>(value);
    } else {
      return parseNextTInstantSet();
    }
  }
  throw new ParseException("Invalid Temporal");
};

template <typename T> unique_ptr<TSequence<T>> Parser<T>::parseNextTSequence() {
  skipWhitespaces();

  if (!hasNext() || !(peek(0) == '[' || peek(0) == '(')) {
    throw ParseException("Expected either a '[' or '('");
  }

  const char opening = peek(0);
  consumeChar(opening);
  const bool left_open = opening == '(';

  vector<unique_ptr<TInstant<T>>> v = {};
  v.push_back(parseNextTInstant());
  skipWhitespaces();

  while (hasNext() && peek(0) == ',') {
    consumeChar(',');
    v.push_back(parseNextTInstant());
    skipWhitespaces();
  }

  if (!hasNext() || !(peek(0) == ']' || peek(0) == ')')) {
    throw ParseException("Expected either a ']' or ')'");
  }

  const char closing = peek(0);
  consumeChar(closing);
  const bool right_open = closing == ')';

  return make_unique<TSequence<T>>(v, left_open, right_open);
};

template <typename T>
unique_ptr<TInstantSet<T>> Parser<T>::parseNextTInstantSet() {
  skipWhitespaces();
  consumeChar('{');
  set<unique_ptr<TInstant<T>>> s = {};
  s.insert(parseNextTInstant());
  skipWhitespaces();

  while (hasNext() && peek(0) == ',') {
    consumeChar(',');
    s.insert(parseNextTInstant());
    skipWhitespaces();
  }

  if (!hasNext() || peek(0) != '}') {
    throw ParseException("Expected a '}'");
  }
  consumeChar('}');

  return make_unique<TInstantSet<T>>(s);
};

template <typename T> unique_ptr<TInstant<T>> Parser<T>::parseNextTInstant() {
  skipWhitespaces();
  T value = parseNextValue();
  string::size_type pos = in.find_first_of("@", iter - in.begin());
  if (pos == string::npos) {
    throw ParseException("Invalid TInstant: needs to contain @");
  }
  consumeChar('@');
  time_t t = parseNextTime();

  return make_unique<TInstant<T>>(value, t);
};

/**
 * Parse time in ISO8601 format
 * Skips initial whitespaces, reads until one of ",)]}\n" is reached, and tries
 * to parse everything in between as time in ISO8601 format
 */
template <typename T> time_t Parser<T>::parseNextTime() {
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
    throw ParseException("Empty or unexpected length for the provided ISO 8601 "
                         "date/time string");
  }

  tm time = {0};
  time.tm_year = parseNextValue() - 1900;
  consumeChar('-');
  time.tm_mon = parseNextValue() - 1;
  consumeChar('-');
  time.tm_mday = parseNextValue();

  int millis = 0;

  if (iter - in.begin() != end_pos) {
    if (*iter != ' ' && (*iter != 'T')) {
      throw ParseException("Expected either a space or a 'T' after day");
    }
    consumeChar(*iter); // skip the character
    time.tm_hour = parseNextValue();
    consumeChar(':');
    time.tm_min = parseNextValue();
    consumeChar(':');
    time.tm_sec = parseNextValue();
    time.tm_isdst = 0;
    millis = input.length() > 20 ? parseNextValue() : 0;
  }

  return timegm(&time) * 1000 + millis;
}

template <typename T> T Parser<T>::parseNextValue() {
  skipWhitespaces();
  try {
    string s = string(iter, in.end());
    string::size_type length;
    if (std::is_same<T, int>::value) {
      int ivalue = stoi(s, &length, 10);
      iter += length;
      return ivalue;
    } else if (std::is_same<T, float>::value) {
      float fvalue = stof(s, &length);
      iter += length;
      return fvalue;
    }
    throw ParseException("Unsupported type");
  } catch (invalid_argument e) {
    throw ParseException("Could not parse: invalid argument");
  } catch (out_of_range e) {
    throw ParseException("Could not parse: out of range");
  }
}

template <typename T> char Parser<T>::peek(int lookahead) {
  if ((in.end() - iter) < lookahead) {
    throw ParseException("Reached end of stream");
  };
  return *(iter + lookahead);
}

template <typename T> void Parser<T>::skipWhitespaces() { skipChars(" \t\n"); }

template <typename T> void Parser<T>::consumeChar(const char c) {
  string::size_type current_pos = iter - in.begin();
  if (*iter != c) {
    std::stringstream error;
    error << "Expected character '" << c << "' at position " << current_pos;
    throw ParseException(error.str());
  }
  iter += 1;
}

template <typename T> void Parser<T>::skipChars(const string &chars) {
  string::size_type current_pos = iter - in.begin();
  string::size_type end_pos = in.find_first_not_of(chars, current_pos);
  if (end_pos == string::npos) {
    end_pos = in.end() - in.begin();
  }
  int length = end_pos - current_pos;
  iter += length;
}

template <typename T> bool Parser<T>::hasNext() { return iter != in.end(); }
