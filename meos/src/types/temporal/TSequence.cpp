#include <iomanip>
#include <meos/types/temporal/TSequence.hpp>

template <typename T>
TSequence<T>::TSequence(vector<unique_ptr<TInstant<T>>> &instants_,
                        bool lower_inc_, bool upper_inc_)
    : lower_inc(lower_inc_), upper_inc(upper_inc_) {
  this->m_instants.reserve(instants_.size());
  for (const auto &e : instants_)
    this->m_instants.push_back(e->clone());
}

template <typename T>
TSequence<T>::TSequence(vector<TInstant<T>> &instants_, bool lower_inc_,
                        bool upper_inc_)
    : lower_inc(lower_inc_), upper_inc(upper_inc_) {
  this->m_instants.reserve(instants_.size());
  for (const auto &e : instants_)
    this->m_instants.push_back(e.clone());
}

template <typename T>
TSequence<T>::TSequence(const TSequence &t)
    : lower_inc(t.lower_inc), upper_inc(t.upper_inc) {
  this->m_instants.reserve(t.m_instants.size());
  for (const auto &e : t.m_instants)
    this->m_instants.push_back(e->clone());
}

template <typename T> vector<TInstant<T>> TSequence<T>::getInstants() const {
  vector<TInstant<T>> s;
  for (const unique_ptr<TInstant<T>> &e : this->m_instants) {
    s.push_back(*e.get());
  }
  return s;
}

template <typename T> set<TInstant<T>> TSequence<T>::instants() const {
  set<TInstant<T>> s;
  for (const unique_ptr<TInstant<T>> &e : this->m_instants) {
    s.insert(*e.get());
  }
  return s;
}

template <typename T> set<time_t> TSequence<T>::timestamps() const {
  set<time_t> s;
  for (const unique_ptr<TInstant<T>> &e : this->m_instants) {
    s.insert(e.get()->getTimestamp());
  }
  return s;
}

template <typename T> const PeriodSet TSequence<T>::getTime() const {
  set<Period> s = {this->period()};
  return PeriodSet(s);
}

template <typename T> const Period TSequence<T>::period() const {
  return Period(this->startTimestamp(), this->endTimestamp(), lower_inc,
                upper_inc);
}

template <typename T>
unique_ptr<TSequence<T>> TSequence<T>::shift(const time_t timedelta) const {
  return unique_ptr<TSequence<T>>(this->shift_impl(timedelta));
}

template <typename T>
TSequence<T> *TSequence<T>::shift_impl(const time_t timedelta) const {
  vector<TInstant<T>> s;
  for (const unique_ptr<TInstant<T>> &e : this->m_instants) {
    s.push_back(TInstant<T>(e->getValue(), e->getTimestamp() + timedelta));
  }
  return new TSequence<T>(s, lower_inc, upper_inc);
}

template <typename T>
bool TSequence<T>::intersectsTimestamp(const time_t datetime) const {
  return this->period().contains_timestamp(datetime);
};

template <typename T>
bool TSequence<T>::intersectsPeriod(const Period period) const {
  return this->period().overlap(period);
};
