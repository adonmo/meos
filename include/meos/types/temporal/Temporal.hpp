#ifndef MEOS_TYPES_TEMPORAL_TEMPORAL_HPP
#define MEOS_TYPES_TEMPORAL_TEMPORAL_HPP

#include <type_traits>

#include <meos/types/geom/Geometry.hpp>
#include <meos/types/geom/SRIDMembers.hpp>
#include <meos/types/range/Range.hpp>
#include <meos/types/temporal/TemporalDuration.hpp>
#include <meos/types/time/Period.hpp>
#include <meos/types/time/PeriodSet.hpp>
#include <meos/types/time/TimestampSet.hpp>

using namespace std;

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

/**
 * Declares an empty type
 */
typedef tuple<> Empty;

/**
 * Temporal is the base class for all the temporal types which are
 * TInstant, TInstantSet, TSequence and TSequenceSet.
 *
 * This is a template class and takes in a BaseType as the template parameter.
 * BaseType can be bool, int, float, string or Geometry.
 *
 * When the BaseType is Geometry, we add additional support for SRIDs.
 * We do this by conditionally inheriting from SRIDMembers.
 */
template <typename BaseType = float>
class Temporal : public conditional_t<is_same<BaseType, Geometry>::value,
                                      SRIDMembers, Empty> {
public:
  Temporal();
  virtual ~Temporal();

  unique_ptr<Temporal<BaseType>> clone() const {
    return unique_ptr<Temporal<BaseType>>(this->clone_impl());
  };

  virtual int compare(Temporal const &other) const = 0;

  /**
   * Duration of the temporal value, that is, one of Instant, InstantSet,
   * Sequence, or SequenceSet.
   */
  virtual TemporalDuration duration() const {
    return TemporalDuration::Temporal;
  };

  /**
   * Set of values taken by the temporal value.
   */
  virtual set<Range<BaseType>> getValues() const = 0;

  /**
   * Minimum value.
   *
   * The function does not take into account whether the bounds are inclusive or
   * not.
   */
  BaseType minValue() const;

  /**
   * Maximum value.
   *
   * The function does not take into account whether the bounds are inclusive or
   * not.
   */
  BaseType maxValue() const;

  /**
   * Period set on which the temporal value is defined.
   */
  virtual PeriodSet getTime() const = 0;

  /**
   * Interval on which the temporal value is defined.
   */
  virtual duration_ms timespan() const = 0;

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
  size_t numTimestamps() const;

  /**
   * Start timestamp.
   */
  time_point startTimestamp() const;

  /**
   * End timestamp.
   */
  time_point endTimestamp() const;

  /**
   * N-th timestamp.
   */
  time_point timestampN(size_t n) const;

  /**
   * Set of timestamps.
   */
  virtual set<time_point> timestamps() const = 0;

  /**
   * Shift the temporal value by a time interval
   */
  unique_ptr<Temporal<BaseType>> shift(duration_ms const timedelta) const;

  /**
   * Does the temporal value intersect the timestamp?
   */
  virtual bool intersectsTimestamp(time_point const datetime) const = 0;

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
  virtual Temporal<BaseType> *clone_impl() const = 0;

  virtual Temporal<BaseType> *shift_impl(duration_ms const timedelta) const = 0;
};

template class Temporal<bool>;
template class Temporal<int>;
template class Temporal<float>;
template class Temporal<string>;
template class Temporal<Geometry>;

#endif
