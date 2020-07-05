#ifndef MEOS_IO_SERIALIZER_HPP
#define MEOS_IO_SERIALIZER_HPP

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

template <typename T = float> class Serializer {
public:
  string write(Temporal<T> const *temporal);
  string write(TInstant<T> const *instant);
  string write(TInstantSet<T> const *instant_set);
  string write(TSequence<T> const *sequence);
  string write(TSequenceSet<T> const *sequence_set);

  string write(Period const *period);
  string write(PeriodSet const *period_set);
  string write(TimestampSet const *timestamp_set);

  string write(T const &value);

  /**
   * Serialize time in ISO8601 format
   */
  string writeTime(time_point const &time);

private:
};

template class Serializer<bool>;
template class Serializer<int>;
template class Serializer<float>;
template class Serializer<string>;
template class Serializer<Geometry>;

#endif
