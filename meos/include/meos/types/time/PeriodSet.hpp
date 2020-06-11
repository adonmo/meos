#ifndef MEOS_TYPES_TIME_PERIODSET_HPP
#define MEOS_TYPES_TIME_PERIODSET_HPP

#include <iomanip>
#include <iterator>
#include <meos/types/time/Period.hpp>
#include <meos/util/time.hpp>
#include <set>

using namespace std;

class PeriodSet {
public:
  PeriodSet(set<unique_ptr<Period>> &periods_);

  PeriodSet(set<Period> &periods_);

  PeriodSet(PeriodSet const &t);

  set<Period> periods() const;
  Period period() const;
  int numPeriods() const;
  Period startPeriod() const;
  Period endPeriod() const;
  Period periodN(int n) const;

  time_t timespan() const;
  unique_ptr<PeriodSet> shift(time_t const timedelta) const;

  set<time_t> timestamps() const;
  int numTimestamps() const;
  time_t startTimestamp() const;
  time_t endTimestamp() const;
  time_t timestampN(int n) const;

  friend bool operator==(PeriodSet const &lhs, PeriodSet const &rhs) {
    return lhs.periods() == rhs.periods();
  }

  friend bool operator!=(PeriodSet const &lhs, PeriodSet const &rhs) {
    return lhs.periods() != rhs.periods();
  }

  friend bool operator<(PeriodSet const &lhs, PeriodSet const &rhs) {
    return lhs.periods() < rhs.periods();
  }

  friend ostream &operator<<(ostream &os, PeriodSet const &period_set) {
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

protected:
  set<unique_ptr<Period>> m_periods;
};

#endif
