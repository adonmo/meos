#include <iomanip>
#include <meos/types/time/PeriodSet.hpp>

PeriodSet::PeriodSet(set<unique_ptr<Period>> &periods_) {
  for (const auto &e : periods_)
    m_periods.insert(e->clone());
}

PeriodSet::PeriodSet(set<Period> &periods_) {
  for (auto e : periods_)
    m_periods.insert(e.clone());
}

PeriodSet::PeriodSet(const PeriodSet &t) {
  for (const auto &e : t.m_periods)
    m_periods.insert(e->clone());
}

set<Period> PeriodSet::periods() const {
  set<Period> s;
  for (const auto &e : m_periods) {
    s.insert(*e.get());
  }
  return s;
}

Period PeriodSet::period() const {
  Period start = startPeriod();
  Period end = endPeriod();
  return Period(start.lower(), end.upper(), start.lower_inc(), end.upper_inc());
}

int PeriodSet::numPeriods() const { return periods().size(); }

Period PeriodSet::startPeriod() const {
  set<Period> s = periods();
  if (s.size() <= 0) {
    throw "At least one period expected";
  }
  return *s.begin();
}

Period PeriodSet::endPeriod() const {
  set<Period> s = periods();
  if (s.size() <= 0) {
    throw "At least one period expected";
  }
  return *s.rbegin();
}

Period PeriodSet::periodN(int n) const {
  set<Period> s = periods();
  if (s.size() < n) {
    throw "At least " + to_string(n) + " period(s) expected";
  }
  return *next(s.begin(), n);
}

const time_t PeriodSet::timespan() const {
  time_t result = 0;
  for (const unique_ptr<Period> &period : m_periods)
    result += period->timespan();
  return result;
}

unique_ptr<PeriodSet> PeriodSet::shift(const time_t timedelta) const {
  set<unique_ptr<Period>> pset;
  for (const auto &e : m_periods)
    pset.insert(e->shift(timedelta));
  return make_unique<PeriodSet>(pset);
}

set<time_t> PeriodSet::timestamps() const {
  set<time_t> s;
  for (const auto &e : m_periods) {
    s.insert(e->lower());
    s.insert(e->upper());
  }
  return s;
}

int PeriodSet::numTimestamps() const { return timestamps().size(); }

time_t PeriodSet::startTimestamp() const {
  set<time_t> s = timestamps();
  if (s.size() <= 0) {
    throw "At least one timestamp expected";
  }
  return *s.begin();
}

time_t PeriodSet::endTimestamp() const {
  set<time_t> s = timestamps();
  if (s.size() <= 0) {
    throw "At least one timestamp expected";
  }
  return *s.rbegin();
}

time_t PeriodSet::timestampN(int n) const {
  set<time_t> s = timestamps();
  if (s.size() < n) {
    throw "At least " + to_string(n) + " timestamp(s) expected";
  }
  return *next(s.begin(), n);
}
