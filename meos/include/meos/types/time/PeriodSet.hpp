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

protected:
  PeriodSet(const PeriodSet &t);
};

#endif
