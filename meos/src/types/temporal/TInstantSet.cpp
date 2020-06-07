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
  set<TInstant<T>> s = {};
  for (const unique_ptr<TInstant<T>> &e : instants) {
    s.insert(*e.get());
  }
  return s;
}

template <typename T> set<time_t> TInstantSet<T>::timestamps() const {
  set<time_t> s = {};
  for (const unique_ptr<TInstant<T>> &e : instants) {
    s.insert(e.get()->getTimestamp());
  }
  return s;
}

template <typename T> const Period TInstantSet<T>::period() const {
  return Period(this->startTimestamp(), this->endTimestamp(), true, true);
};
