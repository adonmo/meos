#include <iomanip>
#include <meos/types/time/TimestampSet.hpp>

TimestampSet::TimestampSet(set<time_t> &timestamps_) {
  for (const auto &e : timestamps_)
    m_timestamps.insert(e);
}

TimestampSet::TimestampSet(const TimestampSet &t) {
  for (const auto &e : t.m_timestamps)
    m_timestamps.insert(e);
}

set<Period> TimestampSet::periods() const {
  set<Period> s;
  for (const auto &e : m_timestamps) {
    s.insert(Period(e, e, true, true));
  }
  return s;
}

Period TimestampSet::period() const {
  Period start = startPeriod();
  Period end = endPeriod();
  return Period(start.lower(), end.upper(), start.lower_inc(), end.upper_inc());
}

int TimestampSet::numPeriods() const { return periods().size(); }

Period TimestampSet::startPeriod() const {
  set<Period> s = periods();
  if (s.size() <= 0) {
    throw "At least one period expected";
  }
  return *s.begin();
}

Period TimestampSet::endPeriod() const {
  set<Period> s = periods();
  if (s.size() <= 0) {
    throw "At least one period expected";
  }
  return *s.rbegin();
}

Period TimestampSet::periodN(int n) const {
  set<Period> s = periods();
  if (s.size() < n) {
    throw "At least " + to_string(n) + " period(s) expected";
  }
  return *next(s.begin(), n);
}

/**
 * Note: timespan is not defined on TimestampSet officially
 */
const time_t TimestampSet::timespan() const { return 0; }

unique_ptr<TimestampSet> TimestampSet::shift(const time_t timedelta) const {
  set<time_t> pset;
  for (const auto &e : m_timestamps)
    pset.insert(e + timedelta);
  return make_unique<TimestampSet>(pset);
}

set<time_t> TimestampSet::timestamps() const {
  set<time_t> s;
  for (const auto &e : m_timestamps) {
    s.insert(e);
  }
  return s;
}

int TimestampSet::numTimestamps() const { return timestamps().size(); }

time_t TimestampSet::startTimestamp() const {
  set<time_t> s = timestamps();
  if (s.size() <= 0) {
    throw "At least one timestamp expected";
  }
  return *s.begin();
}

time_t TimestampSet::endTimestamp() const {
  set<time_t> s = timestamps();
  if (s.size() <= 0) {
    throw "At least one timestamp expected";
  }
  return *s.rbegin();
}

time_t TimestampSet::timestampN(int n) const {
  set<time_t> s = timestamps();
  if (s.size() < n) {
    throw "At least " + to_string(n) + " timestamp(s) expected";
  }
  return *next(s.begin(), n);
}
