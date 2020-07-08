#include <iomanip>
#include <meos/types/temporal/TInstantSet.hpp>

template <typename T> TInstantSet<T>::TInstantSet() {}

template <typename T>
TInstantSet<T>::TInstantSet(set<TInstant<T>> const &instants)
    : m_instants(instants) {}

template <typename T>
TInstantSet<T>::TInstantSet(TInstantSet const &t) : m_instants(t.m_instants) {}

template <typename T> TInstantSet<T>::TInstantSet(set<string> const &instants) {
  for (auto const &e : instants)
    this->m_instants.insert(TInstant<T>(e));
}

template <typename T> TInstantSet<T>::TInstantSet(string const &serialized) {
  stringstream ss(serialized);
  TInstantSet<T> instant_set;
  ss >> instant_set;
  this->m_instants = instant_set.instants();
}

template <typename T>
int TInstantSet<T>::compare(Temporal<T> const &other) const {
  if (this->duration() != other.duration()) {
    throw std::invalid_argument("Unsupported types for comparision");
  }

  TInstantSet<T> const *that = dynamic_cast<TInstantSet<T> const *>(&other);
  // Compare number of instants
  if (this->m_instants.size() < that->m_instants.size())
    return -1;
  if (this->m_instants.size() > that->m_instants.size())
    return 1;

  // Compare instant by instant
  auto lhs_instants = this->instants();
  auto rhs_instants = that->instants();
  auto lhs = lhs_instants.begin();
  auto rhs = rhs_instants.begin();
  while (lhs != lhs_instants.end()) {
    if (*lhs < *rhs)
      return -1;
    if (*lhs > *rhs)
      return 1;
    lhs++;
    rhs++;
  }

  // The two are equal
  return 0;
}

template <typename T> set<TInstant<T>> TInstantSet<T>::instants() const {
  return this->m_instants;
}

template <typename T> duration_ms TInstantSet<T>::timespan() const {
  return duration_ms(0);
}

template <typename T> set<Range<T>> TInstantSet<T>::getValues() const {
  set<Range<T>> s;
  for (auto const &e : this->m_instants) {
    s.insert(Range<T>(e.getValue(), e.getValue(), true, true));
  }
  return s;
}

template <typename T> set<time_point> TInstantSet<T>::timestamps() const {
  set<time_point> s;
  for (auto const &e : this->m_instants) {
    s.insert(e.getTimestamp());
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
    s.insert(TInstant<T>(e.getValue(), e.getTimestamp() + timedelta));
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
}

template <typename T>
bool TInstantSet<T>::intersectsPeriod(Period const period) const {
  for (auto const &t : this->timestamps()) {
    if (period.contains_timestamp(t)) {
      return true;
    }
  }
  return false;
}
