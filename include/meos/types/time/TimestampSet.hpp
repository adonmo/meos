#ifndef MEOS_TYPES_TIME_TIMESTAMPSET_HPP
#define MEOS_TYPES_TIME_TIMESTAMPSET_HPP

#include <iomanip>
#include <iterator>
#include <meos/types/time/Period.hpp>
#include <meos/util/serializing.hpp>
#include <set>

using namespace std;

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

class TimestampSet {
public:
  TimestampSet();
  TimestampSet(set<time_point> &timestamps);
  TimestampSet(set<string> &timestamps);
  TimestampSet(string const serialized);

  set<Period> periods() const;
  Period period() const;
  size_t numPeriods() const;
  Period startPeriod() const;
  Period endPeriod() const;
  Period periodN(size_t n) const;

  duration_ms timespan() const;
  unique_ptr<TimestampSet> shift(duration_ms const timedelta) const;

  set<time_point> timestamps() const;
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

  friend istream &operator>>(istream &in, TimestampSet &timestamp_set);
  friend ostream &operator<<(ostream &os, TimestampSet const &timestamp_set);

protected:
  set<time_point> m_timestamps;
};

#endif
