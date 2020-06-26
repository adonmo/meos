#include <iomanip>
#include <meos/types/temporal/TSequence.hpp>

template <typename T>
TSequence<T>::TSequence(vector<unique_ptr<TInstant<T>>> &instants_,
                        bool lower_inc_, bool upper_inc_)
    : lower_inc(lower_inc_), upper_inc(upper_inc_) {
  this->m_instants.reserve(instants_.size());
  for (auto const &e : instants_)
    this->m_instants.push_back(e->clone());
}

template <typename T>
TSequence<T>::TSequence(vector<TInstant<T>> &instants_, bool lower_inc_,
                        bool upper_inc_)
    : lower_inc(lower_inc_), upper_inc(upper_inc_) {
  this->m_instants.reserve(instants_.size());
  for (auto const &e : instants_)
    this->m_instants.push_back(e.clone());
}

template <typename T>
TSequence<T>::TSequence(TSequence const &t)
    : lower_inc(t.lower_inc), upper_inc(t.upper_inc) {
  this->m_instants.reserve(t.m_instants.size());
  for (auto const &e : t.m_instants)
    this->m_instants.push_back(e->clone());
}

template <typename T> vector<TInstant<T>> TSequence<T>::getInstants() const {
  vector<TInstant<T>> s;
  for (auto const &e : this->m_instants) {
    s.push_back(*e.get());
  }
  return s;
}

template <typename T> set<TInstant<T>> TSequence<T>::instants() const {
  set<TInstant<T>> s;
  for (auto const &e : this->m_instants) {
    s.insert(*e.get());
  }
  return s;
}

template <typename T> set<Range<T>> TSequence<T>::getValues() const {
  if (this->m_instants.size() == 0)
    return {};
  T min = (*this->m_instants.begin())->getValue();
  T max = (*this->m_instants.begin())->getValue();
  for (auto const &e : this->m_instants) {
    if (e->getValue() < min) {
      min = e->getValue();
    }
    if (e->getValue() > max) {
      max = e->getValue();
    }
  }
  return {Range<T>(min, max, this->lower_inc, this->upper_inc)};
}

template <typename T> set<time_point> TSequence<T>::timestamps() const {
  set<time_point> s;
  for (auto const &e : this->m_instants) {
    s.insert(e.get()->getTimestamp());
  }
  return s;
}

template <typename T> PeriodSet TSequence<T>::getTime() const {
  set<Period> s = {this->period()};
  return PeriodSet(s);
}

template <typename T> Period TSequence<T>::period() const {
  return Period(this->startTimestamp(), this->endTimestamp(), lower_inc,
                upper_inc);
}

template <typename T>
unique_ptr<TSequence<T>>
TSequence<T>::shift(duration_ms const timedelta) const {
  return unique_ptr<TSequence<T>>(this->shift_impl(timedelta));
}

template <typename T>
TSequence<T> *TSequence<T>::shift_impl(duration_ms const timedelta) const {
  vector<TInstant<T>> s;
  for (auto const &e : this->m_instants) {
    s.push_back(TInstant<T>(e->getValue(), e->getTimestamp() + timedelta));
  }
  return new TSequence<T>(s, lower_inc, upper_inc);
}

template <typename T>
bool TSequence<T>::intersectsTimestamp(time_point const datetime) const {
  return this->period().contains_timestamp(datetime);
};

template <typename T>
bool TSequence<T>::intersectsPeriod(Period const period) const {
  return this->period().overlap(period);
};
