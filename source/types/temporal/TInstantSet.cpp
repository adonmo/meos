#include <iomanip>
#include <meos/types/temporal/TInstantSet.hpp>

template <typename T>
TInstantSet<T>::TInstantSet(set<unique_ptr<TInstant<T>>> &instants_) {
  for (auto const &e : instants_)
    this->m_instants.insert(e->clone());
}

template <typename T> TInstantSet<T>::TInstantSet(set<TInstant<T>> &instants_) {
  for (auto const &e : instants_)
    this->m_instants.insert(e.clone());
}

template <typename T> TInstantSet<T>::TInstantSet(TInstantSet const &t) {
  for (auto const &e : t.m_instants)
    this->m_instants.insert(e->clone());
}

template <typename T> set<TInstant<T>> TInstantSet<T>::getInstants() const {
  set<TInstant<T>> s;
  for (auto const &e : this->m_instants) {
    s.insert(*e.get());
  }
  return s;
}

template <typename T> set<TInstant<T>> TInstantSet<T>::instants() const {
  set<TInstant<T>> s;
  for (auto const &e : this->m_instants) {
    s.insert(*e.get());
  }
  return s;
}

template <typename T> set<Range<T>> TInstantSet<T>::getValues() const {
  set<Range<T>> s;
  for (auto const &e : this->m_instants) {
    s.insert(Range<T>(e->getValue(), e->getValue(), true, true));
  }
  return s;
}

template <typename T> set<time_point> TInstantSet<T>::timestamps() const {
  set<time_point> s;
  for (auto const &e : this->m_instants) {
    s.insert(e.get()->getTimestamp());
  }
  return s;
}

template <typename T> PeriodSet TInstantSet<T>::getTime() const {
  set<Period> s;
  for (auto const &e : instants()) {
    s.insert(e.period());
  }
  return PeriodSet(s);
}

template <typename T> Period TInstantSet<T>::period() const {
  return Period(this->startTimestamp(), this->endTimestamp(), true, true);
}

template <typename T>
unique_ptr<TInstantSet<T>>
TInstantSet<T>::shift(duration_ms const timedelta) const {
  return unique_ptr<TInstantSet<T>>(this->shift_impl(timedelta));
}

template <typename T>
TInstantSet<T> *TInstantSet<T>::shift_impl(duration_ms const timedelta) const {
  set<TInstant<T>> s;
  for (auto const &e : this->m_instants) {
    s.insert(TInstant<T>(e->getValue(), e->getTimestamp() + timedelta));
  }
  return new TInstantSet<T>(s);
}

template <typename T>
bool TInstantSet<T>::intersectsTimestamp(time_point const datetime) const {
  for (auto const &t : this->timestamps()) {
    if (t == datetime) {
      return true;
    }
  }
  return false;
};

template <typename T>
bool TInstantSet<T>::intersectsPeriod(Period const period) const {
  for (auto const &t : this->timestamps()) {
    if (period.contains_timestamp(t)) {
      return true;
    }
  }
  return false;
};
