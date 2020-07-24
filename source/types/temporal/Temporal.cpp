#include <iomanip>
#include <meos/types/temporal/Temporal.hpp>

template <typename BaseType> Temporal<BaseType>::Temporal() {}

template <typename BaseType> Temporal<BaseType>::~Temporal() {}

template <typename BaseType> BaseType Temporal<BaseType>::minValue() const {
  set<Range<BaseType>> s = getValues();
  if (s.size() <= 0) {
    throw "At least one value expected";
  }
  return s.begin()->lower();
}

template <typename BaseType> BaseType Temporal<BaseType>::maxValue() const {
  set<Range<BaseType>> s = getValues();
  if (s.size() <= 0) {
    throw "At least one value expected";
  }
  return s.rbegin()->upper();
}

template <typename BaseType> size_t Temporal<BaseType>::numTimestamps() const {
  return timestamps().size();
}

template <typename BaseType> time_point Temporal<BaseType>::startTimestamp() const {
  set<time_point> s = timestamps();
  if (s.size() <= 0) {
    throw "At least one timestamp expected";
  }
  return *s.begin();
}

template <typename BaseType> time_point Temporal<BaseType>::endTimestamp() const {
  set<time_point> s = timestamps();
  if (s.size() <= 0) {
    throw "At least one timestamp expected";
  }
  return *s.rbegin();
}

template <typename BaseType> time_point Temporal<BaseType>::timestampN(size_t n) const {
  set<time_point> s = timestamps();
  if (s.size() <= n) {
    throw "At least " + to_string(n) + " timestamp(s) expected";
  }
  return *next(s.begin(), n);
}

template <typename BaseType>
unique_ptr<Temporal<BaseType>> Temporal<BaseType>::shift(duration_ms const timedelta) const {
  return unique_ptr<Temporal<BaseType>>(this->shift_impl(timedelta));
}

template <typename BaseType>
bool Temporal<BaseType>::intersectsTimestampSet(TimestampSet const timestampset) const {
  for (auto const &t : timestampset.timestamps()) {
    if (intersectsTimestamp(t)) {
      return true;
    }
  }
  return false;
}

template <typename BaseType>
bool Temporal<BaseType>::intersectsPeriodSet(PeriodSet const periodset) const {
  for (auto const &p : periodset.periods()) {
    if (intersectsPeriod(p)) {
      return true;
    }
  }
  return false;
}

template class Temporal<bool>;
template class Temporal<int>;
template class Temporal<float>;
template class Temporal<string>;
template class Temporal<Geometry>;
