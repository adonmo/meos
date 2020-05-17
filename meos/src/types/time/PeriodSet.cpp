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
