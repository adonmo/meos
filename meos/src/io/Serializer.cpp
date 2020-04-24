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
  if (const int *point = reinterpret_cast<const int *>(&value)) {
    return to_string(value);
  } else if (const float *point = reinterpret_cast<const float *>(&value)) {
    return to_string(value);
  }
  throw SerializationException("Unsupported type");
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
  ss << "{";
  for (auto const &instant : instant_set->instants) {
    ss << write(instant.get());
  }
  ss << "}";
  return ss.str();
}

template <typename T> string Serializer<T>::write(TSequence<T> *sequence) {
  stringstream ss;
  ss << (sequence->left_open ? "(" : "[");
  for (auto const &instant : sequence->instants) {
    ss << write(instant.get());
  }
  ss << (sequence->right_open ? ")" : "]");
  return ss.str();
}
