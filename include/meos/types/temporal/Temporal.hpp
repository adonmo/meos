#pragma once

#include <meos/types/geom/GeomPoint.hpp>
#include <meos/types/geom/SRIDMembers.hpp>
#include <meos/types/range/Range.hpp>
#include <meos/types/temporal/TemporalDuration.hpp>
#include <meos/types/time/Period.hpp>
#include <meos/types/time/PeriodSet.hpp>
#include <meos/types/time/TimestampSet.hpp>
#include <tuple>
#include <type_traits>

namespace meos {

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

/**
 * Declares an empty type
 */
typedef std::tuple<> Empty;

/**
 * @brief Temporal is the base class for all the temporal types.
 *
 * This is a template class and takes in a BaseType as the template parameter.
 * BaseType can be bool, int, float, string or GeomPoint.
 *
 * When the BaseType is GeomPoint, we add additional support for SRIDs.
 * We do this by conditionally inheriting from SRIDMembers.
 */
template <typename BaseType = float> class Temporal
    : public std::conditional_t<std::is_same<BaseType, GeomPoint>::value, SRIDMembers, Empty> {
public:
  Temporal();
  virtual ~Temporal();

  std::unique_ptr<Temporal<BaseType>> clone() const {
    return std::unique_ptr<Temporal<BaseType>>(this->clone_impl());
  };

  virtual int compare(Temporal const &other) const = 0;

  /**
   * @brief Duration of the temporal value, that is, one of Instant, InstantSet,
   * Sequence, or SequenceSet.
   * @returns The temporal duration of the current object.
   */
  virtual TemporalDuration duration() const { return TemporalDuration::Temporal; };

  /**
   * @brief Set of values taken by the temporal value.
   */
  virtual std::set<Range<BaseType>> getValues() const = 0;

  /**
   * @brief Minimum value, irrespective of whether the bounds are inclusive or not.
   */
  BaseType minValue() const;

  /**
   * @brief Maximum value, irrespective of whether the bounds are inclusive or not.
   */
  BaseType maxValue() const;

  /**
   * @brief Period set on which the temporal value is defined.
   */
  virtual PeriodSet getTime() const = 0;

  /**
   * @brief Interval on which the temporal value is defined.
   */
  virtual duration_ms timespan() const = 0;

  /**
   * @brief Period on which the temporal value is defined ignoring potential time gaps.
   */
  virtual Period period() const = 0;

  /**
   * @brief Set of instants.
   */
  std::set<Temporal> instants() const;

  /**
   * @brief Number of distinct timestamps.
   */
  size_t numTimestamps() const;

  /**
   * @brief Start timestamp.
   */
  time_point startTimestamp() const;

  /**
   * @brief End timestamp.
   */
  time_point endTimestamp() const;

  /**
   * @brief N-th timestamp.
   */
  time_point timestampN(size_t n) const;

  /**
   * @brief Set of timestamps.
   */
  virtual std::set<time_point> timestamps() const = 0;

  /**
   * @brief Shift the temporal value by a time interval
   */
  std::unique_ptr<Temporal<BaseType>> shift(duration_ms const timedelta) const;

  /**
   * @brief Does the temporal value intersect the timestamp?
   */
  virtual bool intersectsTimestamp(time_point const datetime) const = 0;

  /**
   * @brief Does the temporal value intersect the timestamp set?
   */
  bool intersectsTimestampSet(TimestampSet const timestampset) const;

  /**
   * @brief Does the temporal value intersect the period?
   */
  virtual bool intersectsPeriod(Period const period) const = 0;

  /**
   * @brief Does the temporal value intersect the period set?
   */
  bool intersectsPeriodSet(PeriodSet const periodset) const;

private:
  virtual Temporal<BaseType> *clone_impl() const = 0;

  virtual Temporal<BaseType> *shift_impl(duration_ms const timedelta) const = 0;
};

typedef Temporal<bool> TBool;
typedef Temporal<int> TInt;
typedef Temporal<float> TFloat;
typedef Temporal<std::string> TText;
typedef Temporal<GeomPoint> TGeomPoint;

}  // namespace meos
