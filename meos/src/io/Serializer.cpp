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

template <typename T>
string Serializer<T>::write(unique_ptr<Temporal<T>> &temporal) {
  stringstream ss;
  if (TInstant<T> *instant = static_cast<TInstant<T> *>(temporal.get())) {
    ss << write(instant->getValue()) << "@" << writeTime(instant->getT());
    return ss.str();
  } else if (const TInstantSet<T> *instant_set =
                 static_cast<TInstantSet<T> *>(temporal.get())) {
    return "instant_set";
  } else if (const TSequence<T> *sequence =
                 static_cast<TSequence<T> *>(temporal.get())) {
    return "sequence";
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
