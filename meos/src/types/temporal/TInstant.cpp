#include <iomanip>
#include <meos/types/temporal/TInstant.hpp>

template <typename T>
TInstant<T>::TInstant(T value_, std::time_t t_) : value(value_), t(t_) {}

template <typename T> const T &TInstant<T>::getValue() const {
  return this->value;
}

template <typename T> const std::time_t &TInstant<T>::getTimestamp() const {
  return this->t;
}

template <typename T> set<time_t> TInstant<T>::timestamps() const {
  return {getTimestamp()};
}

template <typename T> const Period TInstant<T>::period() const {
  return Period(this->getTimestamp(), this->getTimestamp(), true, true);
};

template <typename T>
unique_ptr<TInstant<T>> TInstant<T>::shift(const time_t timedelta) const {
  return make_unique<TInstant<T>>(this->getValue(),
                                  this->getTimestamp() + timedelta);
}
