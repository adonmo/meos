#ifndef MEOS_TYPES_TEMPORAL_TEMPORAL_HPP
#define MEOS_TYPES_TEMPORAL_TEMPORAL_HPP

#include <meos/types/geom/Geometry.hpp>
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

  // /**
  //  * Set of values taken by the temporal value.
  //  */
  // virtual set<unique_ptr<T>> getValues() const;

  // /**
  //  * Start value.
  //  */
  // virtual unique_ptr<T> startValue() const;

  // /**
  //  * End value.
  //  */
  // virtual unique_ptr<T> endValue() const;

  // /**
  //  * Minimum value.
  //  */
  // virtual unique_ptr<T> minValue() const;

  // /**
  //  * Maximum value.
  //  */
  // virtual unique_ptr<T> maxValue() const;

  /**
   * Period set on which the temporal value is defined.
   */
  virtual const PeriodSet getTime() const = 0;

  /**
   * Interval on which the temporal value is defined.
   */
  const time_t timespan() const;

  /**
   * Period on which the temporal value is defined ignoring potential time gaps.
   */
  virtual const Period period() const = 0;

  /**
   * Set of instants.
   */
  set<Temporal> instants() const;

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

  /**
   * Shift the temporal value by a time interval
   */
  unique_ptr<Temporal<T>> shift(const time_t timedelta) const;

  /**
   * Does the temporal value intersect the timestamp?
   */
  virtual bool intersectsTimestamp(const time_t datetime) const = 0;

  /**
   * Does the temporal value intersect the timestamp set?
   */
  bool intersectsTimestampSet(const TimestampSet timestampset) const;

  /**
   * Does the temporal value intersect the period?
   */
  virtual bool intersectsPeriod(const Period period) const = 0;

  /**
   * Does the temporal value intersect the period set?
   */
  bool intersectsPeriodSet(const PeriodSet periodset) const;

private:
  virtual Temporal<T> *clone_impl() const = 0;

  virtual Temporal<T> *shift_impl(const time_t timedelta) const = 0;
};

template class Temporal<bool>;
template class Temporal<int>;
template class Temporal<float>;
template class Temporal<string>;
template class Temporal<Geometry>;

#endif
