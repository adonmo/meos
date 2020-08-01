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

class TimestampSet {
public:
  TimestampSet();
  TimestampSet(std::set<time_point> &timestamps);
  TimestampSet(std::set<std::string> &timestamps);
  TimestampSet(std::string const &serialized);

  std::set<Period> periods() const;
  Period period() const;
  size_t numPeriods() const;
  Period startPeriod() const;
  Period endPeriod() const;
  Period periodN(size_t n) const;

  duration_ms timespan() const;
  std::unique_ptr<TimestampSet> shift(duration_ms const timedelta) const;

  std::set<time_point> timestamps() const;
  size_t numTimestamps() const;
  time_point startTimestamp() const;
  time_point endTimestamp() const;
  time_point timestampN(size_t n) const;

  friend bool operator==(TimestampSet const &lhs, TimestampSet const &rhs);
  friend bool operator!=(TimestampSet const &lhs, TimestampSet const &rhs);
  friend bool operator<(TimestampSet const &lhs, TimestampSet const &rhs);
  friend bool operator>(TimestampSet const &lhs, TimestampSet const &rhs);
  friend bool operator>=(TimestampSet const &lhs, TimestampSet const &rhs);
  friend bool operator<=(TimestampSet const &lhs, TimestampSet const &rhs);

  friend std::istream &operator>>(std::istream &in, TimestampSet &timestamp_set);
  friend std::ostream &operator<<(std::ostream &os, TimestampSet const &timestamp_set);

protected:
  std::set<time_point> m_timestamps;
};

}  // namespace meos
