#ifndef MEOS_TYPES_TEMPORAL_TEMPORAL_HPP
#define MEOS_TYPES_TEMPORAL_TEMPORAL_HPP

#include <meos/types/geom/Geometry.hpp>
#include <meos/types/range/Range.hpp>
#include <meos/types/time/Period.hpp>
#include <meos/types/time/PeriodSet.hpp>
#include <meos/types/time/TimestampSet.hpp>

using namespace std;

template <typename T = float> class Temporal {
public:
  Temporal();

  unique_ptr<Temporal<T>> clone() const {
    return unique_ptr<Temporal<T>>(this->clone_impl());
  };

  /**
   * Duration of the temporal value, that is, one of Instant, InstantSet,
   * Sequence, or SequenceSet.
   */
  virtual string duration() const { return "Temporal"; };

  /**
   * Set of values taken by the temporal value.
   */
  virtual set<Range<T>> getValues() const = 0;

  /**
   * Minimum value.
   *
   * The function does not take into account whether the bounds are inclusive or
   * not.
   */
  T minValue() const;

  /**
   * Maximum value.
   *
   * The function does not take into account whether the bounds are inclusive or
   * not.
   */
  T maxValue() const;

  /**
   * Period set on which the temporal value is defined.
   */
  virtual PeriodSet getTime() const = 0;

  /**
   * Interval on which the temporal value is defined.
   */
  time_t timespan() const;

  /**
   * Period on which the temporal value is defined ignoring potential time gaps.
   */
  virtual Period period() const = 0;

  /**
   * Set of instants.
   */
  set<Temporal> instants() const;

  /**
   * Number of distinct timestamps.
   */
  int numTimestamps() const;

  /**
   * Start timestamp.
   */
  time_t startTimestamp() const;

  /**
   * End timestamp.
   */
  time_t endTimestamp() const;

  /**
   * N-th timestamp.
   */
  time_t timestampN(int n) const;

  /**
   * Set of timestamps.
   */
  virtual set<time_t> timestamps() const = 0;

  /**
   * Shift the temporal value by a time interval
   */
  unique_ptr<Temporal<T>> shift(time_t const timedelta) const;

  /**
   * Does the temporal value intersect the timestamp?
   */
  virtual bool intersectsTimestamp(time_t const datetime) const = 0;

  /**
   * Does the temporal value intersect the timestamp set?
   */
  bool intersectsTimestampSet(TimestampSet const timestampset) const;

  /**
   * Does the temporal value intersect the period?
   */
  virtual bool intersectsPeriod(Period const period) const = 0;

  /**
   * Does the temporal value intersect the period set?
   */
  bool intersectsPeriodSet(PeriodSet const periodset) const;

private:
  virtual Temporal<T> *clone_impl() const = 0;

  virtual Temporal<T> *shift_impl(time_t const timedelta) const = 0;
};

template class Temporal<bool>;
template class Temporal<int>;
template class Temporal<float>;
template class Temporal<string>;
template class Temporal<Geometry>;

#endif
