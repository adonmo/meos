#ifndef MEOS_TYPES_TEMPORAL_TEMPORAL_HPP
#define MEOS_TYPES_TEMPORAL_TEMPORAL_HPP

#include <meos/types/geom/Geometry.hpp>
#include <meos/types/time/Period.hpp>
#include <meos/types/time/PeriodSet.hpp>

using namespace std;

template <typename T = float> class Temporal {
private:
public:
  Temporal();

  /**
   * Interval on which the temporal value is defined.
   */
  const time_t timespan() const;

  /**
   * Period on which the temporal value is defined ignoring potential time gaps.
   */
  virtual const Period period() const = 0;

  /**
   * Number of distinct timestamps.
   */
  const int numTimestamps() const;

  /**
   * Start timestamp.
   */
  const time_t startTimestamp() const;

  /**
   * End timestamp.
   */
  const time_t endTimestamp() const;

  /**
   * N-th timestamp.
   */
  const time_t timestampN(int n) const;

  /**
   * Set of timestamps.
   */
  virtual set<time_t> timestamps() const = 0;
};

template class Temporal<bool>;
template class Temporal<int>;
template class Temporal<float>;
template class Temporal<string>;
template class Temporal<Geometry>;

#endif
