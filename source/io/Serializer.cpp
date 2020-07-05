#include <meos/io/SerializationException.hpp>
#include <meos/io/Serializer.hpp>
#include <meos/util/serializing.hpp>

using namespace std;

template <typename T> string Serializer<T>::write(Temporal<T> const *temporal) {
  stringstream ss;
  if (TInstant<T> const *instant =
          reinterpret_cast<TInstant<T> const *>(temporal)) {
    return write(instant);
  } else if (TInstantSet<T> const *instant_set =
                 reinterpret_cast<TInstantSet<T> const *>(temporal)) {
    return write(instant_set);
  } else if (TSequence<T> const *sequence =
                 reinterpret_cast<TSequence<T> const *>(temporal)) {
    return write(sequence);
  } else if (TSequenceSet<T> const *sequence_set =
                 reinterpret_cast<TSequenceSet<T> const *>(temporal)) {
    return write(sequence_set);
  }
  throw SerializationException("Unsupported type");
}

template <typename T> string Serializer<T>::write(T const &value) {
  // Check specialized template functions below for supported types
  throw SerializationException("Unsupported type");
}

template <> string Serializer<bool>::write(bool const &value) {
  return value ? "t" : "f";
}

template <> string Serializer<int>::write(int const &value) {
  return to_string(value);
}

template <> string Serializer<float>::write(float const &value) {
  stringstream ss;
  ss << value;
  return ss.str();
}

template <> string Serializer<string>::write(string const &value) {
  return '"' + value + '"';
}

template <> string Serializer<Geometry>::write(Geometry const &value) {
  return value.toWKT();
}

template <typename T> string Serializer<T>::writeTime(time_point const &t) {
  return write_ISO8601_time(t);
}

template <typename T> string Serializer<T>::write(TInstant<T> const *instant) {
  stringstream ss;
  ss << *instant;
  return ss.str();
}

template <typename T>
string Serializer<T>::write(TInstantSet<T> const *instant_set) {
  stringstream ss;
  ss << *instant_set;
  return ss.str();
}

template <typename T>
string Serializer<T>::write(TSequence<T> const *sequence) {
  stringstream ss;
  ss << *sequence;
  return ss.str();
}

template <typename T>
string Serializer<T>::write(TSequenceSet<T> const *sequence_set) {
  stringstream ss;
  ss << *sequence_set;
  return ss.str();
}

template <typename T> string Serializer<T>::write(Period const *period) {
  stringstream ss;
  ss << *period;
  return ss.str();
}

template <typename T> string Serializer<T>::write(PeriodSet const *period_set) {
  stringstream ss;
  ss << *period_set;
  return ss.str();
}

template <typename T>
string Serializer<T>::write(TimestampSet const *timestamp_set) {
  stringstream ss;
  ss << *timestamp_set;
  return ss.str();
}
