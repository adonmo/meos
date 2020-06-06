#ifndef MEOS_TYPES_TIME_PERIODSET_HPP
#define MEOS_TYPES_TIME_PERIODSET_HPP

#include <iomanip>
#include <meos/types/time/Period.hpp>
#include <set>

using namespace std;

class PeriodSet {
public:
  set<unique_ptr<Period>> periods;

  PeriodSet(set<unique_ptr<Period>> &periods_);

  PeriodSet(set<Period> &periods_);

  set<Period> getPeriods();

  const time_t timespan();
  unique_ptr<PeriodSet> shift(const time_t timedelta);

  set<time_t> timestamps();
  int numTimestamps();
  time_t startTimestamp();
  time_t endTimestamp();
  time_t timestampN(int n);

protected:
  PeriodSet(const PeriodSet &t);
};

#endif
