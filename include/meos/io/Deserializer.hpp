#pragma once

#include <iomanip>
#include <meos/types/geom/GeomPoint.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantSet.hpp>
#include <meos/types/temporal/TSequence.hpp>
#include <meos/types/temporal/TSequenceSet.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/types/time/Period.hpp>
#include <meos/types/time/PeriodSet.hpp>
#include <meos/types/time/TimestampSet.hpp>
#include <string>

namespace meos {

template <typename T = float> class Deserializer {
public:
  Deserializer(std::string const &in);
  std::unique_ptr<Temporal<T>> nextTemporal();
  std::unique_ptr<TSequenceSet<T>> nextTSequenceSet();
  std::unique_ptr<TSequence<T>> nextTSequence();
  std::unique_ptr<TInstantSet<T>> nextTInstantSet();
  std::unique_ptr<TInstant<T>> nextTInstant();

  std::unique_ptr<Period> nextPeriod();
  std::unique_ptr<PeriodSet> nextPeriodSet();
  std::unique_ptr<TimestampSet> nextTimestampSet();

  /**
   * Deserialize time in ISO8601 format
   */
  time_point nextTime();
  T nextValue();

private:
  std::string const in;
  std::string::const_iterator iter;
  int nextInt();
  char peek(int lookahead);
  void skipWhitespaces();
  void consumeChar(char const c);
  void skipChars(std::string const &chars);
  bool hasNext();
};

}  // namespace meos
