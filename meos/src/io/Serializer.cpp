#include <iostream>
#include <meos/io/SerializationException.hpp>
#include <meos/io/Serializer.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantSet.hpp>
#include <meos/types/temporal/TSequence.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <sstream>
#include <string>

using namespace std;

template <typename T> string Serializer<T>::write(Temporal<T> *temporal) {
  stringstream ss;
  if (TInstant<T> *instant = reinterpret_cast<TInstant<T> *>(temporal)) {
    return write(instant);
  } else if (TInstantSet<T> *instant_set =
                 reinterpret_cast<TInstantSet<T> *>(temporal)) {
    return write(instant_set);
  } else if (TSequence<T> *sequence =
                 reinterpret_cast<TSequence<T> *>(temporal)) {
    return write(sequence);
  }
  throw SerializationException("Unsupported type");
}

template <typename T> string Serializer<T>::write(const T &value) {
  if (std::is_same<T, bool>::value) {
    const bool *v = reinterpret_cast<const bool *>(&value);
    return *v ? "t" : "f";
  } else if (std::is_same<T, int>::value) {
    const int *v = reinterpret_cast<const int *>(&value);
    return to_string(*v);
  } else if (std::is_same<T, float>::value) {
    const float *v = reinterpret_cast<const float *>(&value);
    return to_string(*v);
  } else if (std::is_same<T, string>::value) {
    const string *v = reinterpret_cast<const string *>(&value);
    return '"' + *v + '"';
  }
  throw SerializationException("Unsupported type");
}

template <>
string Serializer<GEOSGeometry *>::write(GEOSGeometry *const &value) {
  GEOSWKTWriter *wktw_ = GEOSWKTWriter_create();
  GEOSWKTWriter_setTrim(wktw_, 1);
  GEOSWKTWriter_setRoundingPrecision(wktw_, 8);
  return string(GEOSWKTWriter_write(wktw_, value));
}

template <typename T> string Serializer<T>::writeTime(const time_t &t) {
  stringstream textStream;
  // TODO: FIXME as of now, milliseconds are lost
  time_t tt(t);
  tt = tt / 1000;
  textStream << put_time(gmtime(&tt), "%FT%T%z");
  return textStream.str();
}

template <typename T> string Serializer<T>::write(TInstant<T> *instant) {
  stringstream ss;
  ss << write(instant->getValue()) << "@" << writeTime(instant->getT());
  return ss.str();
}

template <typename T> string Serializer<T>::write(TInstantSet<T> *instant_set) {
  stringstream ss;
  bool first = true;
  ss << "{";
  for (auto const &instant : instant_set->instants) {
    if (first)
      first = false;
    else
      ss << ", ";
    ss << write(instant.get());
  }
  ss << "}";
  return ss.str();
}

template <typename T> string Serializer<T>::write(TSequence<T> *sequence) {
  stringstream ss;
  bool first = true;
  ss << (sequence->left_open ? "(" : "[");
  for (auto const &instant : sequence->instants) {
    if (first)
      first = false;
    else
      ss << ", ";
    ss << write(instant.get());
  }
  ss << (sequence->right_open ? ")" : "]");
  return ss.str();
}
