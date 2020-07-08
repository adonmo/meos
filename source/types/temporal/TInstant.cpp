#include <iomanip>
#include <meos/types/temporal/TInstant.hpp>

template <typename T> TInstant<T>::TInstant() {}

template <typename T>
TInstant<T>::TInstant(T value_, time_point t_) : value(value_), t(t_) {}

template <typename T>
TInstant<T>::TInstant(pair<T, time_point> p) : value(p.first), t(p.second) {}

template <typename T>
TInstant<T>::TInstant(string const value, string const t) {
  stringstream lss(value);
  this->value = nextValue<T>(lss);
  stringstream uss(t);
  this->t = nextTime(uss);
}

template <typename T>
TInstant<T>::TInstant(pair<string const, string const> p) {
  stringstream lss(p.first);
  this->value = nextValue<T>(lss);
  stringstream uss(p.second);
  this->t = nextTime(uss);
}

template <typename T> TInstant<T>::TInstant(string const serialized) {
  stringstream ss(serialized);
  TInstant<T> instant;
  ss >> instant;
  this->value = instant.value;
  this->t = instant.t;
}

template <typename T> int TInstant<T>::compare(Temporal<T> const &other) const {
  if (this->duration() != other.duration()) {
    throw std::invalid_argument("Unsupported types for comparision");
  }

  TInstant<T> const *that = dynamic_cast<TInstant<T> const *>(&other);

  // Compare timestamps
  if (this->t < that->t)
    return -1;
  if (this->t > that->t)
    return 1;

  // Compare values
  if (this->value < that->value)
    return -1;
  if (this->value > that->value)
    return 1;

  // The two are equal
  return 0;
}

template <typename T> T TInstant<T>::getValue() const { return this->value; }

template <typename T> time_point TInstant<T>::getTimestamp() const {
  return this->t;
}

template <typename T> set<TInstant<T>> TInstant<T>::instants() const {
  return {*this};
}

template <typename T> duration_ms TInstant<T>::timespan() const {
  return duration_ms(0);
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
}

template <typename T>
unique_ptr<TInstant<T>> TInstant<T>::shift(duration_ms const timedelta) const {
  return unique_ptr<TInstant<T>>(this->shift_impl(timedelta));
}

template <typename T> TInstant<T> *TInstant<T>::clone_impl() const {
  return new TInstant<T>(this->value, this->t);
}

template <typename T>
TInstant<T> *TInstant<T>::shift_impl(duration_ms const timedelta) const {
  return new TInstant<T>(this->getValue(), this->getTimestamp() + timedelta);
}

template <typename T>
bool TInstant<T>::intersectsTimestamp(time_point const datetime) const {
  return datetime == this->t;
}

template <typename T>
bool TInstant<T>::intersectsPeriod(Period const period) const {
  return period.contains_timestamp(this->t);
}
