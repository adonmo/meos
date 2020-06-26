#include <iomanip>
#include <meos/types/temporal/TInstant.hpp>

template <typename T>
TInstant<T>::TInstant(T value_, time_point t_) : value(value_), t(t_) {}

template <typename T> T TInstant<T>::getValue() const { return this->value; }

template <typename T> time_point TInstant<T>::getTimestamp() const {
  return this->t;
}

template <typename T> set<TInstant<T>> TInstant<T>::instants() const {
  return {*this};
}

template <typename T> set<Range<T>> TInstant<T>::getValues() const {
  return {Range<T>(this->getValue(), this->getValue(), true, true)};
}

template <typename T> set<time_point> TInstant<T>::timestamps() const {
  return {getTimestamp()};
}

template <typename T> PeriodSet TInstant<T>::getTime() const {
  set<Period> s = {this->period()};
  return PeriodSet(s);
}

template <typename T> Period TInstant<T>::period() const {
  return Period(this->getTimestamp(), this->getTimestamp(), true, true);
};

template <typename T>
unique_ptr<TInstant<T>> TInstant<T>::shift(duration_ms const timedelta) const {
  return unique_ptr<TInstant<T>>(this->shift_impl(timedelta));
}

template <typename T>
TInstant<T> *TInstant<T>::shift_impl(duration_ms const timedelta) const {
  return new TInstant<T>(this->getValue(), this->getTimestamp() + timedelta);
}

template <typename T>
bool TInstant<T>::intersectsTimestamp(time_point const datetime) const {
  return datetime == this->t;
};

template <typename T>
bool TInstant<T>::intersectsPeriod(Period const period) const {
  return period.contains_timestamp(this->t);
};
