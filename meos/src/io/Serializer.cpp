#include <meos/io/SerializationException.hpp>
#include <meos/io/Serializer.hpp>
#include <string>

using namespace std;

template <typename T>
string Serializer<T>::write(unique_ptr<Temporal<T>> temporal) {
  throw SerializationException("Not implemented yet");
}

template <typename T> string Serializer<T>::write(const T &value) {
  if (const int *point = reinterpret_cast<const int *>(&value)) {
    return to_string(value);
  } else if (const float *point = reinterpret_cast<const float *>(&value)) {
    return to_string(value);
  }
  throw SerializationException("Unsupported type");
}

template <typename T> string Serializer<T>::write(const time_t &t) {
  stringstream textStream;
  textStream << put_time(gmtime(&t), "%FT%T%z");
  return textStream.str();
}
