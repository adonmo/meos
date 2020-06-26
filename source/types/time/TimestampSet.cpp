#include <iomanip>
#include <meos/types/time/TimestampSet.hpp>

TimestampSet::TimestampSet(set<time_point> &timestamps_) {
  for (auto const &e : timestamps_)
    m_timestamps.insert(e);
}

TimestampSet::TimestampSet(TimestampSet const &t) {
  for (auto const &e : t.m_timestamps)
    m_timestamps.insert(e);
}

set<Period> TimestampSet::periods() const {
  set<Period> s;
  for (auto const &e : m_timestamps) {
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
 * Note: timespan is not defined on TimestampSet in official MobilityDB's spec
 */
duration_ms TimestampSet::timespan() const { return duration_ms(0); }

unique_ptr<TimestampSet>
TimestampSet::shift(duration_ms const timedelta) const {
  set<time_point> pset;
  for (auto const &e : m_timestamps)
    pset.insert(e + timedelta);
  return make_unique<TimestampSet>(pset);
}

set<time_point> TimestampSet::timestamps() const {
  set<time_point> s;
  for (auto const &e : m_timestamps) {
    s.insert(e);
  }
  return s;
}

int TimestampSet::numTimestamps() const { return timestamps().size(); }

time_point TimestampSet::startTimestamp() const {
  set<time_point> s = timestamps();
  if (s.size() <= 0) {
    throw "At least one timestamp expected";
  }
  return *s.begin();
}

time_point TimestampSet::endTimestamp() const {
  set<time_point> s = timestamps();
  if (s.size() <= 0) {
    throw "At least one timestamp expected";
  }
  return *s.rbegin();
}

time_point TimestampSet::timestampN(int n) const {
  set<time_point> s = timestamps();
  if (s.size() < n) {
    throw "At least " + to_string(n) + " timestamp(s) expected";
  }
  return *next(s.begin(), n);
}
