#ifndef MEOS_IO_DESERIALIZER_HPP
#define MEOS_IO_DESERIALIZER_HPP

#include <iomanip>
#include <meos/types/geom/Geometry.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantSet.hpp>
#include <meos/types/temporal/TSequence.hpp>
#include <meos/types/temporal/TSequenceSet.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/types/time/Period.hpp>
#include <meos/types/time/PeriodSet.hpp>
#include <meos/types/time/TimestampSet.hpp>
#include <string>

using namespace std;

template <typename T = float> class Deserializer {
public:
  Deserializer(string const &in);
  unique_ptr<Temporal<T>> nextTemporal();
  unique_ptr<TSequenceSet<T>> nextTSequenceSet();
  unique_ptr<TSequence<T>> nextTSequence();
  unique_ptr<TInstantSet<T>> nextTInstantSet();
  unique_ptr<TInstant<T>> nextTInstant();

  unique_ptr<Period> nextPeriod();
  unique_ptr<PeriodSet> nextPeriodSet();
  unique_ptr<TimestampSet> nextTimestampSet();

  /**
   * Deserialize time in ISO8601 format
   */
  time_point nextTime();
  T nextValue();

private:
  string const in;
  string::const_iterator iter;
  int nextInt();
  char peek(int lookahead);
  void skipWhitespaces();
  void consumeChar(char const c);
  void skipChars(string const &chars);
  bool hasNext();
};

template class Deserializer<bool>;
template class Deserializer<int>;
template class Deserializer<float>;
template class Deserializer<string>;
template class Deserializer<Geometry>;

#endif
