#include <iomanip>
#include <meos/types/temporal/Temporal.hpp>

template <typename T> Temporal<T>::Temporal() {}

template <typename T> Temporal<T>::~Temporal() {}

template <typename T> T Temporal<T>::minValue() const {
  set<Range<T>> s = getValues();
  if (s.size() <= 0) {
    throw "At least one value expected";
  }
  return s.begin()->lower();
}

template <typename T> T Temporal<T>::maxValue() const {
  set<Range<T>> s = getValues();
  if (s.size() <= 0) {
    throw "At least one value expected";
  }
  return s.rbegin()->upper();
}

template <typename T> size_t Temporal<T>::numTimestamps() const {
  return timestamps().size();
}

template <typename T> time_point Temporal<T>::startTimestamp() const {
  set<time_point> s = timestamps();
  if (s.size() <= 0) {
    throw "At least one timestamp expected";
  }
  return *s.begin();
}

template <typename T> time_point Temporal<T>::endTimestamp() const {
  set<time_point> s = timestamps();
  if (s.size() <= 0) {
    throw "At least one timestamp expected";
  }
  return *s.rbegin();
}

template <typename T> time_point Temporal<T>::timestampN(size_t n) const {
  set<time_point> s = timestamps();
  if (s.size() <= n) {
    throw "At least " + to_string(n) + " timestamp(s) expected";
  }
  return *next(s.begin(), n);
}

template <typename T>
unique_ptr<Temporal<T>> Temporal<T>::shift(duration_ms const timedelta) const {
  return unique_ptr<Temporal<T>>(this->shift_impl(timedelta));
}

template <typename T>
bool Temporal<T>::intersectsTimestampSet(
    TimestampSet const timestampset) const {
  for (auto const &t : timestampset.timestamps()) {
    if (intersectsTimestamp(t)) {
      return true;
    }
  }
  return false;
}

template <typename T>
bool Temporal<T>::intersectsPeriodSet(PeriodSet const periodset) const {
  for (auto const &p : periodset.periods()) {
    if (intersectsPeriod(p)) {
      return true;
    }
  }
  return false;
}
