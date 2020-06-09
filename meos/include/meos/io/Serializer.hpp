#ifndef MEOS_IO_SERIALIZER_HPP
#define MEOS_IO_SERIALIZER_HPP

#include <iomanip>
#include <meos/types/geom/Geometry.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantSet.hpp>
#include <meos/types/temporal/TSequence.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/types/time/Period.hpp>
#include <meos/types/time/PeriodSet.hpp>
#include <meos/types/time/TimestampSet.hpp>
#include <string>

using namespace std;

template <typename T = float> class Serializer {
public:
  string write(const Temporal<T> *temporal);
  string write(const TInstant<T> *instant);
  string write(const TInstantSet<T> *instant_set);
  string write(const TSequence<T> *sequence);

  string write(const Period *period);
  string write(const PeriodSet *period_set);
  string write(const TimestampSet *timestamp_set);

  string write(const T &value);

  /**
   * Serialize time in ISO8601 format
   */
  string writeTime(const time_t &time);

private:
};

template class Serializer<bool>;
template class Serializer<int>;
template class Serializer<float>;
template class Serializer<string>;
template class Serializer<Geometry>;

#endif
