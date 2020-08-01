#include <algorithm>
#include <ctime>
#include <meos/io/DeserializationException.hpp>
#include <meos/io/Deserializer.hpp>
#include <meos/io/utils.hpp>
#include <sstream>
#include <string>

using namespace std;

template <typename T> Deserializer<T>::Deserializer(string const &in_) : in(in_) {
  iter = in.begin();
}

template <typename T> unique_ptr<Temporal<T>> Deserializer<T>::nextTemporal() {
  skipWhitespaces();
  auto current_pos = iter - in.begin();

  // We use two lookaheads to decide along the way
  // Temporals are at least 2 characters long, so its safe to do this
  char lookahead1 = peek(0);
  char lookahead2 = peek(1);

  // In case SRID prefix (SRID=...;) is present
  if (lookahead1 == 'S') {
    current_pos = in.find_first_of(';', current_pos);
    lookahead1 = in.at(current_pos + 1);
    lookahead2 = in.at(current_pos + 2);
  }

  // In case interpolation prefix (Interp=...;) is present
  // This means we are looking at either a Sequence or a Sequence Set
  if (lookahead1 == 'I') {
    current_pos = in.find_first_of(';', current_pos);
    lookahead1 = in.at(current_pos + 1);
    lookahead2 = in.at(current_pos + 2);

    if (lookahead1 == '{') {
      return nextTSequenceSet();
    } else {
      return nextTSequence();
    }

    throw new DeserializationException("Invalid Temporal");
  }

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

template <typename T> unique_ptr<TSequenceSet<T>> Deserializer<T>::nextTSequenceSet() {
  string::size_type current_pos = iter - in.begin();
  string s = in.substr(current_pos, 2048);
  stringstream ss(s);
  TSequenceSet<T> t;
  ss >> t;
  iter += ss.tellg();
  return make_unique<TSequenceSet<T>>(t);
}

template <typename T> unique_ptr<TSequence<T>> Deserializer<T>::nextTSequence() {
  string::size_type current_pos = iter - in.begin();
  string s = in.substr(current_pos, 2048);
  stringstream ss(s);
  TSequence<T> t;
  ss >> t;
  iter += ss.tellg();
  return make_unique<TSequence<T>>(t);
}

template <typename T> unique_ptr<TInstantSet<T>> Deserializer<T>::nextTInstantSet() {
  string::size_type current_pos = iter - in.begin();
  string s = in.substr(current_pos, 2048);
  stringstream ss(s);
  TInstantSet<T> t;
  ss >> t;
  iter += ss.tellg();
  return make_unique<TInstantSet<T>>(t);
}

template <typename T> unique_ptr<TInstant<T>> Deserializer<T>::nextTInstant() {
  string::size_type current_pos = iter - in.begin();
  string s = in.substr(current_pos, 2048);
  stringstream ss(s);
  TInstant<T> t;
  ss >> t;
  iter += ss.tellg();
  return make_unique<TInstant<T>>(t);
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

template <typename T> unique_ptr<TimestampSet> Deserializer<T>::nextTimestampSet() {
  string::size_type current_pos = iter - in.begin();
  string s = in.substr(current_pos, 2048);
  stringstream ss(s);
  TimestampSet timestamp_set;
  ss >> timestamp_set;
  iter += ss.tellg();
  return make_unique<TimestampSet>(timestamp_set);
}

template <typename T> time_point Deserializer<T>::nextTime() {
  string::size_type current_pos = iter - in.begin();
  string s = in.substr(current_pos, 256);
  stringstream ss(s);
  auto t = ::nextTime(ss);
  iter += ss.tellg();
  return t;
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
  size_t length = end_pos - current_pos;
  string input = in.substr(current_pos, length);
  transform(input.begin(), input.end(), input.begin(), ::tolower);

  bool value;
  if (input == "t" || input == "true") {
    value = true;
  } else if (input == "f" || input == "false") {
    value = false;
  } else {
    throw DeserializationException("Boolean value can only be one of (t, f, true, false), but got: "
                                   + input);
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
  size_t length = end_pos - current_pos;
  string input = in.substr(current_pos, length);

  // Skip double quotes if present
  if (length >= 2 && input[0] == '"' && input[input.length() - 1] == '"') {
    input = input.substr(1, length - 2);
  }

  if (length <= 0) {
    throw DeserializationException("Could not parse text: empty, unquoted value");
  }

  iter += length;
  return input;
}

template <> GeomPoint Deserializer<GeomPoint>::nextValue() {
  string::size_type current_pos = iter - in.begin();
  string s = in.substr(current_pos, 2048);
  stringstream ss(s);
  GeomPoint value;
  ss >> value;
  iter += ss.tellg();
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

template <typename T> void Deserializer<T>::skipWhitespaces() { skipChars(" \t\n"); }

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
  size_t length = end_pos - current_pos;
  iter += length;
}

template <typename T> bool Deserializer<T>::hasNext() { return iter != in.end(); }

template class Deserializer<bool>;
template class Deserializer<int>;
template class Deserializer<float>;
template class Deserializer<string>;
template class Deserializer<GeomPoint>;
