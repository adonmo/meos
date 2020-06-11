#ifndef MEOS_TYPES_TEMPORAL_TINSTANT_HPP
#define MEOS_TYPES_TEMPORAL_TINSTANT_HPP

#include <meos/types/geom/Geometry.hpp>
#include <meos/types/temporal/TInstantFunctions.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/types/time/Period.hpp>
#include <meos/util/time.hpp>

using namespace std;

template <typename T>
class TInstant : public Temporal<T>,
                 public TInstantFunctions<TInstant<T>, TInstant<T>> {
public:
  TInstant(T value, time_t t);
  T getValue() const;
  time_t getTimestamp() const;

  unique_ptr<TInstant<T>> clone() const {
    return std::unique_ptr<TInstant<T>>(this->clone_impl());
  }

  friend bool operator==(TInstant const &lhs, TInstant const &rhs) {
    return (lhs.value == rhs.value) && (lhs.t == rhs.t);
  }

  friend bool operator!=(TInstant const &lhs, TInstant const &rhs) {
    return !(lhs == rhs);
  }

  friend bool operator<(TInstant const &lhs, TInstant const &rhs) {
    return (lhs.t < rhs.t) || ((lhs.t == rhs.t) && (lhs.value < rhs.value));
  }

  friend ostream &operator<<(ostream &os, TInstant const &instant) {
    os << instant.getValue() << "@" << ISO8601_time(instant.getTimestamp());
    return os;
  }

  string duration() const { return "Instant"; };

  /**
   * Set of instants.
   */
  set<TInstant> instants() const;

  set<time_t> timestamps() const override;
  PeriodSet getTime() const;
  Period period() const override;
  unique_ptr<TInstant<T>> shift(time_t const timedelta) const;
  bool intersectsTimestamp(time_t const datetime) const override;
  bool intersectsPeriod(Period const period) const override;

private:
  T const value;
  time_t const t;

  TInstant<T> *clone_impl() const override {
    return new TInstant<T>(this->value, this->t);
  };

  TInstant<T> *shift_impl(time_t const timedelta) const override;
};

template class TInstant<bool>;
template class TInstant<int>;
template class TInstant<float>;
template class TInstant<string>;
template class TInstant<Geometry>;

#endif
