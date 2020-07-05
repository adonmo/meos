#include <iomanip>
#include <meos/io/utils.hpp>
#include <meos/types/time/PeriodSet.hpp>

PeriodSet::PeriodSet() {}

PeriodSet::PeriodSet(set<unique_ptr<Period>> const &periods) {
  for (auto const &e : periods)
    m_periods.insert(e->clone());
}

PeriodSet::PeriodSet(set<Period> const &periods) {
  for (auto e : periods)
    m_periods.insert(e.clone());
}

PeriodSet::PeriodSet(PeriodSet const &t) {
  for (auto const &e : t.m_periods)
    m_periods.insert(e->clone());
}

PeriodSet::PeriodSet(set<string> const &periods) {
  PeriodSet period_set;
  for (auto const &e : periods)
    m_periods.insert(make_unique<Period>(e));
}

PeriodSet::PeriodSet(string const &serialized) {
  stringstream ss(serialized);
  PeriodSet period_set;
  ss >> period_set;
  for (auto const &e : period_set.m_periods)
    m_periods.insert(e->clone());
}

unique_ptr<PeriodSet> PeriodSet::clone() {
  return make_unique<PeriodSet>(this->m_periods);
}

set<Period> PeriodSet::periods() const {
  set<Period> s;
  for (auto const &e : m_periods) {
    s.insert(*e.get());
  }
  return s;
}

Period PeriodSet::period() const {
  Period start = startPeriod();
  Period end = endPeriod();
  return Period(start.lower(), end.upper(), start.lower_inc(), end.upper_inc());
}

size_t PeriodSet::numPeriods() const { return periods().size(); }

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

Period PeriodSet::periodN(size_t n) const {
  set<Period> s = periods();
  if (s.size() <= n) {
    throw "At least " + to_string(n) + " period(s) expected";
  }
  return *next(s.begin(), n);
}

duration_ms PeriodSet::timespan() const {
  duration_ms result(0);
  for (auto const &period : m_periods)
    result += period->timespan();
  return result;
}

unique_ptr<PeriodSet> PeriodSet::shift(duration_ms const timedelta) const {
  set<unique_ptr<Period>> pset;
  for (auto const &e : m_periods)
    pset.insert(e->shift(timedelta));
  return make_unique<PeriodSet>(pset);
}

set<time_point> PeriodSet::timestamps() const {
  set<time_point> s;
  for (auto const &e : m_periods) {
    s.insert(e->lower());
    s.insert(e->upper());
  }
  return s;
}

size_t PeriodSet::numTimestamps() const { return timestamps().size(); }

time_point PeriodSet::startTimestamp() const {
  set<time_point> s = timestamps();
  if (s.size() <= 0) {
    throw "At least one timestamp expected";
  }
  return *s.begin();
}

time_point PeriodSet::endTimestamp() const {
  set<time_point> s = timestamps();
  if (s.size() <= 0) {
    throw "At least one timestamp expected";
  }
  return *s.rbegin();
}

time_point PeriodSet::timestampN(size_t n) const {
  set<time_point> s = timestamps();
  if (s.size() <= n) {
    throw "At least " + to_string(n) + " timestamp(s) expected";
  }
  return *next(s.begin(), n);
}

bool operator==(PeriodSet const &lhs, PeriodSet const &rhs) {
  return lhs.periods() == rhs.periods();
}

bool operator!=(PeriodSet const &lhs, PeriodSet const &rhs) {
  return lhs.periods() != rhs.periods();
}

bool operator<(PeriodSet const &lhs, PeriodSet const &rhs) {
  return lhs.periods() < rhs.periods();
}

bool operator>(PeriodSet const &lhs, PeriodSet const &rhs) { return rhs < lhs; }

bool operator>=(PeriodSet const &lhs, PeriodSet const &rhs) {
  return !(lhs < rhs);
}

bool operator<=(PeriodSet const &lhs, PeriodSet const &rhs) {
  return !(rhs < lhs);
}

istream &operator>>(istream &in, PeriodSet &period_set) {
  char c;

  consume(in, '{');

  set<unique_ptr<Period>> s = {};

  Period period;
  in >> period;
  s.insert(period.clone());

  while (true) {
    in >> c;
    if (c != ',')
      break;
    in >> period;
    s.insert(period.clone());
  }

  if (c != '}') {
    throw invalid_argument("Expected '}'");
  }

  period_set.m_periods.empty();
  for (auto const &e : s)
    period_set.m_periods.insert(e->clone());

  return in;
}

ostream &operator<<(ostream &os, PeriodSet const &period_set) {
  bool first = true;
  os << "{";
  for (auto period : period_set.periods()) {
    if (first)
      first = false;
    else
      os << ", ";
    os << period;
  }
  os << "}";
  return os;
}
