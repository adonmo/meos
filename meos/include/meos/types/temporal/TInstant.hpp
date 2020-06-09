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
  const T &getValue() const;
  const time_t &getTimestamp() const;

  unique_ptr<TInstant<T>> clone() const {
    return std::unique_ptr<TInstant<T>>(this->clone_impl());
  }

  friend bool operator==(const TInstant<T> &lhs, const TInstant<T> &rhs) {
    return (lhs.value == rhs.value) && (lhs.t == rhs.t);
  }

  friend bool operator!=(const TInstant<T> &lhs, const TInstant<T> &rhs) {
    return !(lhs == rhs);
  }

  friend bool operator<(const TInstant<T> &lhs, const TInstant<T> &rhs) {
    return (lhs.t < rhs.t) || ((lhs.t == rhs.t) && (lhs.value < rhs.value));
  }

  friend ostream &operator<<(ostream &os, const TInstant<T> &instant) {
    os << instant.getValue() << "@" << ISO8601_time(instant.getTimestamp());
    return os;
  }

  string duration() const { return "Instant"; };

  /**
   * Set of instants.
   */
  set<TInstant> instants() const;

  set<time_t> timestamps() const override;
  const PeriodSet getTime() const;
  const Period period() const override;
  unique_ptr<TInstant<T>> shift(const time_t timedelta) const;
  bool intersectsTimestamp(const time_t datetime) const override;
  bool intersectsPeriod(const Period period) const override;

private:
  const T value;
  const time_t t;

  TInstant<T> *clone_impl() const override {
    return new TInstant<T>(this->value, this->t);
  };

  TInstant<T> *shift_impl(const time_t timedelta) const override;
};

template class TInstant<bool>;
template class TInstant<int>;
template class TInstant<float>;
template class TInstant<string>;
template class TInstant<Geometry>;

#endif
