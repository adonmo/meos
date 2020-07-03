#ifndef MEOS_TYPES_TIME_PERIODSET_HPP
#define MEOS_TYPES_TIME_PERIODSET_HPP

#include <iomanip>
#include <iterator>
#include <meos/types/time/Period.hpp>
#include <meos/util/serializing.hpp>
#include <set>

using namespace std;

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

class PeriodSet {
public:
  PeriodSet();
  PeriodSet(set<unique_ptr<Period>> const &periods);
  PeriodSet(set<Period> const &periods);
  PeriodSet(set<string> const &periods);
  PeriodSet(PeriodSet const &t);
  PeriodSet(string const &serialized);

  unique_ptr<PeriodSet> clone();

  set<Period> periods() const;
  Period period() const;
  size_t numPeriods() const;
  Period startPeriod() const;
  Period endPeriod() const;
  Period periodN(size_t n) const;

  duration_ms timespan() const;
  unique_ptr<PeriodSet> shift(duration_ms const timedelta) const;

  set<time_point> timestamps() const;
  size_t numTimestamps() const;
  time_point startTimestamp() const;
  time_point endTimestamp() const;
  time_point timestampN(size_t n) const;

  friend bool operator==(PeriodSet const &lhs, PeriodSet const &rhs);
  friend bool operator!=(PeriodSet const &lhs, PeriodSet const &rhs);
  friend bool operator<(PeriodSet const &lhs, PeriodSet const &rhs);
  friend bool operator>(PeriodSet const &lhs, PeriodSet const &rhs);
  friend bool operator>=(PeriodSet const &lhs, PeriodSet const &rhs);
  friend bool operator<=(PeriodSet const &lhs, PeriodSet const &rhs);

  friend istream &operator>>(istream &in, PeriodSet &period_set);
  friend ostream &operator<<(ostream &os, PeriodSet const &period_set);

protected:
  set<unique_ptr<Period>> m_periods;
};

#endif
