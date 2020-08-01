#pragma once

#include <iomanip>
#include <iterator>
#include <meos/types/time/Period.hpp>
#include <meos/util/serializing.hpp>
#include <set>
#include <string>

namespace meos {

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

class PeriodSet {
public:
  PeriodSet();
  PeriodSet(std::set<std::unique_ptr<Period>> const &periods);
  PeriodSet(std::set<Period> const &periods);
  PeriodSet(std::set<std::string> const &periods);
  PeriodSet(PeriodSet const &t);
  PeriodSet(std::string const &serialized);

  std::unique_ptr<PeriodSet> clone();

  std::set<Period> periods() const;
  Period period() const;
  size_t numPeriods() const;
  Period startPeriod() const;
  Period endPeriod() const;
  Period periodN(size_t n) const;

  duration_ms timespan() const;
  std::unique_ptr<PeriodSet> shift(duration_ms const timedelta) const;

  std::set<time_point> timestamps() const;
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

  friend std::istream &operator>>(std::istream &in, PeriodSet &period_set);
  friend std::ostream &operator<<(std::ostream &os, PeriodSet const &period_set);

protected:
  std::set<std::unique_ptr<Period>> m_periods;
};

}  // namespace meos
