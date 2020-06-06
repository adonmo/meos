#include <iomanip>
#include <meos/types/time/PeriodSet.hpp>

PeriodSet::PeriodSet(set<unique_ptr<Period>> &periods_) {
  for (const auto &e : periods_)
    periods.insert(e->clone());
}

PeriodSet::PeriodSet(set<Period> &periods_) {
  for (auto e : periods_)
    periods.insert(e.clone());
}

PeriodSet::PeriodSet(const PeriodSet &t) {
  for (const auto &e : t.periods)
    periods.insert(e->clone());
}

set<Period> PeriodSet::getPeriods() {
  set<Period> s = {};
  for (const unique_ptr<Period> &e : periods) {
    s.insert(*e.get());
  }
  return s;
}

const time_t PeriodSet::timespan() {
  time_t result = 0;
  for (const unique_ptr<Period> &period : periods)
    result += period->timespan();
  return result;
}

unique_ptr<PeriodSet> PeriodSet::shift(const time_t timedelta) {
  set<unique_ptr<Period>> pset;
  for (const auto &e : periods)
    pset.insert(e->shift(timedelta));
  return make_unique<PeriodSet>(pset);
}

set<time_t> PeriodSet::timestamps() {
  set<time_t> tset;
  for (const auto &e : periods) {
    tset.insert(e->lower());
    tset.insert(e->upper());
  }
  return tset;
}

int PeriodSet::numTimestamps() { return timestamps().size(); }

time_t PeriodSet::startTimestamp() {
  set<time_t> tset = timestamps();
  if (tset.size() <= 0) {
    throw "At least one timestamp expected";
  }
  return *tset.begin();
}

time_t PeriodSet::endTimestamp() {
  set<time_t> tset = timestamps();
  if (tset.size() <= 0) {
    throw "At least one timestamp expected";
  }
  return *tset.rbegin();
}

time_t PeriodSet::timestampN(int n) {
  set<time_t> tset = timestamps();
  if (tset.size() < n) {
    throw "At least " + to_string(n) + " timestamp(s) expected";
  }
  return *next(tset.begin(), n);
}
