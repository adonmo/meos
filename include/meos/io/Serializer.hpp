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

template <typename T = float> class Serializer {
public:
  std::string write(Temporal<T> const *temporal);
  std::string write(TInstant<T> const *instant);
  std::string write(TInstantSet<T> const *instant_set);
  std::string write(TSequence<T> const *sequence);
  std::string write(TSequenceSet<T> const *sequence_set);

  std::string write(Period const *period);
  std::string write(PeriodSet const *period_set);
  std::string write(TimestampSet const *timestamp_set);

  std::string write(T const &value);

  /**
   * Serialize time in ISO8601 format
   */
  std::string writeTime(time_point const &time);
};

}  // namespace meos
