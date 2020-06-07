#ifndef MEOS_TYPES_TEMPORAL_TINSTANT_HPP
#define MEOS_TYPES_TEMPORAL_TINSTANT_HPP

#include <meos/types/geom/Geometry.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/types/time/Period.hpp>

using namespace std;

template <typename T = float> class TInstant : public Temporal<T> {
private:
  const T value;
  const time_t t;

public:
  TInstant(T value, time_t t);
  const T &getValue() const;
  const time_t &getTimestamp() const;

  virtual unique_ptr<TInstant> clone() { return make_unique<TInstant>(*this); }

  friend bool operator==(const TInstant<T> &lhs, const TInstant<T> &rhs) {
    return (lhs.value == rhs.value) && (lhs.t == rhs.t);
  }

  friend bool operator!=(const TInstant<T> &lhs, const TInstant<T> &rhs) {
    return !(lhs == rhs);
  }

  friend bool operator<(const TInstant<T> &lhs, const TInstant<T> &rhs) {
    return (lhs.value < rhs.value) ||
           ((lhs.value == rhs.value) && (lhs.t < rhs.t));
  }

  set<time_t> timestamps() const override;
  const Period period() const override;
  unique_ptr<TInstant<T>> shift(const time_t timedelta) const;
  TInstant<T> *shift_impl(const time_t timedelta) const override;
};

template class TInstant<bool>;
template class TInstant<int>;
template class TInstant<float>;
template class TInstant<string>;
template class TInstant<Geometry>;

#endif
