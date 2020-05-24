#include <iomanip>
#include <meos/types/temporal/TInstant.hpp>

template <typename T>
TInstant<T>::TInstant(T value_, std::time_t t_) : value(value_), t(t_) {}

template <typename T> const T &TInstant<T>::getValue() const {
  return this->value;
}

template <typename T> const std::time_t &TInstant<T>::getT() const {
  return this->t;
}
