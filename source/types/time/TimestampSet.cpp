#include <iomanip>
#include <meos/io/utils.hpp>
#include <meos/types/time/TimestampSet.hpp>

TimestampSet::TimestampSet() {}

TimestampSet::TimestampSet(set<time_point> &timestamps) {
  for (auto const &e : timestamps)
    m_timestamps.insert(e);
}

TimestampSet::TimestampSet(set<string> &timestamps) {
  for (auto const &e : timestamps) {
    stringstream ss(e);
    m_timestamps.insert(nextTime(ss));
  }
}

TimestampSet::TimestampSet(string const serialized) {
  stringstream ss(serialized);
  TimestampSet timestamp_set;
  ss >> timestamp_set;
  for (auto const &e : timestamp_set.m_timestamps)
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

size_t TimestampSet::numPeriods() const { return periods().size(); }

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

Period TimestampSet::periodN(size_t n) const {
  set<Period> s = periods();
  if (s.size() <= n) {
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

size_t TimestampSet::numTimestamps() const { return timestamps().size(); }

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

time_point TimestampSet::timestampN(size_t n) const {
  set<time_point> s = timestamps();
  if (s.size() <= n) {
    throw "At least " + to_string(n) + " timestamp(s) expected";
  }
  return *next(s.begin(), n);
}

bool operator==(TimestampSet const &lhs, TimestampSet const &rhs) {
  return lhs.timestamps() == rhs.timestamps();
}

bool operator!=(TimestampSet const &lhs, TimestampSet const &rhs) {
  return lhs.timestamps() != rhs.timestamps();
}

bool operator<(TimestampSet const &lhs, TimestampSet const &rhs) {
  return lhs.timestamps() < rhs.timestamps();
}

bool operator>(TimestampSet const &lhs, TimestampSet const &rhs) {
  return rhs < lhs;
}

bool operator>=(TimestampSet const &lhs, TimestampSet const &rhs) {
  return !(lhs < rhs);
}

bool operator<=(TimestampSet const &lhs, TimestampSet const &rhs) {
  return !(rhs < lhs);
}

istream &operator>>(istream &in, TimestampSet &timestamp_set) {
  char c;

  consume(in, '{');

  set<time_point> s = {};

  s.insert(nextTime(in));

  while (true) {
    in >> c;
    if (c != ',')
      break;
    s.insert(nextTime(in));
  }

  if (c != '}') {
    throw invalid_argument("Expected '}'");
  }

  timestamp_set.m_timestamps = s;

  return in;
}

ostream &operator<<(ostream &os, TimestampSet const &timestamp_set) {
  bool first = true;
  os << "{";
  for (auto t : timestamp_set.timestamps()) {
    if (first)
      first = false;
    else
      os << ", ";
    os << write_ISO8601_time(t);
  }
  os << "}";
  return os;
}
