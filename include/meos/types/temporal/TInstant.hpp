#ifndef MEOS_TYPES_TEMPORAL_TINSTANT_HPP
#define MEOS_TYPES_TEMPORAL_TINSTANT_HPP

#include <meos/io/utils.hpp>
#include <meos/types/geom/Geometry.hpp>
#include <meos/types/temporal/TInstantFunctions.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/types/temporal/TemporalComparators.hpp>
#include <meos/util/serializing.hpp>

using namespace std;

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

template <typename T>
class TInstant : public Temporal<T>,
                 public TemporalComparators<TInstant<T>>,
                 public TInstantFunctions<TInstant<T>, TInstant<T>, T> {
public:
  TInstant();
  TInstant(T value, time_point t);
  TInstant(pair<T, time_point> p);
  TInstant(string const value, string const t);
  TInstant(pair<string const, string const> p);
  TInstant(string const serialized);

  friend istream &operator>>(istream &in, TInstant &instant) {
    instant.value = nextValue<T>(in);
    consume(in, '@');
    instant.t = nextTime(in);
    return in;
  }

  friend ostream &operator<<(ostream &os, TInstant const &instant) {
    os << write_value(instant.getValue()) << "@"
       << write_ISO8601_time(instant.getTimestamp());
    return os;
  }

  int compare(Temporal<T> const &other) const override;

  T getValue() const;
  time_point getTimestamp() const;

  unique_ptr<TInstant<T>> clone() const {
    return std::unique_ptr<TInstant<T>>(this->clone_impl());
  }

  TemporalDuration duration() const { return TemporalDuration::Instant; };

  /**
   * Set of instants.
   */
  set<TInstant> instants() const;

  duration_ms timespan() const override;
  set<Range<T>> getValues() const override;
  set<time_point> timestamps() const override;
  PeriodSet getTime() const;
  Period period() const override;
  unique_ptr<TInstant> shift(duration_ms const timedelta) const;
  bool intersectsTimestamp(time_point const datetime) const override;
  bool intersectsPeriod(Period const period) const override;

private:
  T value;
  time_point t;

  TInstant *clone_impl() const override;
  TInstant *shift_impl(duration_ms const timedelta) const override;
};

template class TInstant<bool>;
template class TInstant<int>;
template class TInstant<float>;
template class TInstant<string>;
template class TInstant<Geometry>;

#endif
