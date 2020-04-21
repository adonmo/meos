#ifndef MEOS_IO_DESERIALIZER_HPP
#define MEOS_IO_DESERIALIZER_HPP

#include <iomanip>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantSet.hpp>
#include <meos/types/temporal/TSequence.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <string>

using namespace std;

template <typename T> class Deserializer {
public:
  Deserializer(const string &in);
  unique_ptr<Temporal<T>> nextTemporal();
  unique_ptr<TSequence<T>> nextTSequence();
  unique_ptr<TInstantSet<T>> nextTInstantSet();
  unique_ptr<TInstant<T>> nextTInstant();

  /**
   * Deserialize time in ISO8601 format
   */
  time_t nextTime();
  T nextValue();

private:
  const string in;
  string::const_iterator iter;
  char peek(int lookahead);
  void skipWhitespaces();
  void consumeChar(const char c);
  void skipChars(const string &chars);
  bool hasNext();
};

template class Deserializer<int>;
template class Deserializer<float>;

#endif
