#ifndef MEOS_TYPES_TIME_PERIODSET_HPP
#define MEOS_TYPES_TIME_PERIODSET_HPP

#include <iomanip>
#include <iterator>
#include <meos/types/time/Period.hpp>
#include <meos/util/time.hpp>
#include <set>

using namespace std;

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

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

  duration_ms timespan() const;
  unique_ptr<PeriodSet> shift(duration_ms const timedelta) const;

  set<time_point> timestamps() const;
  int numTimestamps() const;
  time_point startTimestamp() const;
  time_point endTimestamp() const;
  time_point timestampN(int n) const;

  friend bool operator==(PeriodSet const &lhs, PeriodSet const &rhs);
  friend bool operator!=(PeriodSet const &lhs, PeriodSet const &rhs);
  friend bool operator<(PeriodSet const &lhs, PeriodSet const &rhs);
  friend bool operator>(PeriodSet const &lhs, PeriodSet const &rhs);
  friend bool operator>=(PeriodSet const &lhs, PeriodSet const &rhs);
  friend bool operator<=(PeriodSet const &lhs, PeriodSet const &rhs);

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
