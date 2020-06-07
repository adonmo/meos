#include <iomanip>
#include <meos/types/temporal/TSequence.hpp>

template <typename T>
TSequence<T>::TSequence(vector<unique_ptr<TInstant<T>>> &instants_,
                        bool lower_inc_, bool upper_inc_)
    : lower_inc(lower_inc_), upper_inc(upper_inc_) {
  instants.reserve(instants_.size());
  for (const auto &e : instants_)
    instants.push_back(e->clone());
}

template <typename T>
TSequence<T>::TSequence(vector<TInstant<T>> &instants_, bool lower_inc_,
                        bool upper_inc_)
    : lower_inc(lower_inc_), upper_inc(upper_inc_) {
  instants.reserve(instants_.size());
  for (auto e : instants_)
    instants.push_back(e.clone());
}

template <typename T>
TSequence<T>::TSequence(const TSequence &t)
    : lower_inc(t.lower_inc), upper_inc(t.upper_inc) {
  instants.reserve(t.instants.size());
  for (const auto &e : t.instants)
    instants.push_back(e->clone());
}

template <typename T> vector<TInstant<T>> TSequence<T>::getInstants() const {
  vector<TInstant<T>> s = {};
  for (const unique_ptr<TInstant<T>> &e : instants) {
    s.push_back(*e.get());
  }
  return s;
}

template <typename T> set<time_t> TSequence<T>::timestamps() const {
  set<time_t> s = {};
  for (const unique_ptr<TInstant<T>> &e : instants) {
    s.insert(e.get()->getTimestamp());
  }
  return s;
}

template <typename T> const Period TSequence<T>::period() const {
  return Period(this->startTimestamp(), this->endTimestamp(), lower_inc,
                upper_inc);
};
