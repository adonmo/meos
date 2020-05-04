#ifndef MEOS_TYPES_TEMPORAL_TINSTANT_HPP
#define MEOS_TYPES_TEMPORAL_TINSTANT_HPP

#include <meos/types/geom/Geometry.hpp>
#include <meos/types/temporal/Temporal.hpp>

using namespace std;

template <typename T> class TInstant : public Temporal<T> {
private:
  T value;
  time_t t;

public:
  TInstant(T value, time_t t);
  T getValue();
  time_t &getT();

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
};

template class TInstant<bool>;
template class TInstant<int>;
template class TInstant<float>;
template class TInstant<string>;
template class TInstant<Geometry>;

#endif
