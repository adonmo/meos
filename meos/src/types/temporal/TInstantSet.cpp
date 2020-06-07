#include <iomanip>
#include <meos/types/temporal/TInstantSet.hpp>

template <typename T>
TInstantSet<T>::TInstantSet(set<unique_ptr<TInstant<T>>> &instants_) {
  for (const auto &e : instants_)
    instants.insert(e->clone());
}

template <typename T> TInstantSet<T>::TInstantSet(set<TInstant<T>> &instants_) {
  for (auto e : instants_)
    instants.insert(e.clone());
}

template <typename T> TInstantSet<T>::TInstantSet(const TInstantSet &t) {
  for (const auto &e : t.instants)
    instants.insert(e->clone());
}

template <typename T> set<TInstant<T>> TInstantSet<T>::getInstants() const {
  set<TInstant<T>> s;
  for (const unique_ptr<TInstant<T>> &e : instants) {
    s.insert(*e.get());
  }
  return s;
}

template <typename T> set<time_t> TInstantSet<T>::timestamps() const {
  set<time_t> s;
  for (const unique_ptr<TInstant<T>> &e : instants) {
    s.insert(e.get()->getTimestamp());
  }
  return s;
}

template <typename T> const Period TInstantSet<T>::period() const {
  return Period(this->startTimestamp(), this->endTimestamp(), true, true);
}

template <typename T>
unique_ptr<TInstantSet<T>> TInstantSet<T>::shift(const time_t timedelta) const {
  return unique_ptr<TInstantSet<T>>(this->shift_impl(timedelta));
}

template <typename T>
TInstantSet<T> *TInstantSet<T>::shift_impl(const time_t timedelta) const {
  set<TInstant<T>> s;
  for (const unique_ptr<TInstant<T>> &e : instants) {
    s.insert(TInstant<T>(e->getValue(), e->getTimestamp() + timedelta));
  }
  return new TInstantSet<T>(s);
}
