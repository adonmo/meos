#ifndef MEOS_TYPES_TIME_TIMESTAMPSET_HPP
#define MEOS_TYPES_TIME_TIMESTAMPSET_HPP

#include <iomanip>
#include <iterator>
#include <meos/types/time/Period.hpp>
#include <meos/util/time.hpp>
#include <set>

using namespace std;

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

class TimestampSet {
public:
  TimestampSet(set<time_point> &timestamps_);

  TimestampSet(TimestampSet const &t);

  set<Period> periods() const;
  Period period() const;
  int numPeriods() const;
  Period startPeriod() const;
  Period endPeriod() const;
  Period periodN(int n) const;

  duration_ms timespan() const;
  unique_ptr<TimestampSet> shift(duration_ms const timedelta) const;

  set<time_point> timestamps() const;
  int numTimestamps() const;
  time_point startTimestamp() const;
  time_point endTimestamp() const;
  time_point timestampN(int n) const;

  friend bool operator==(TimestampSet const &lhs, TimestampSet const &rhs) {
    return lhs.timestamps() == rhs.timestamps();
  }

  friend bool operator!=(TimestampSet const &lhs, TimestampSet const &rhs) {
    return lhs.timestamps() != rhs.timestamps();
  }

  friend bool operator<(TimestampSet const &lhs, TimestampSet const &rhs) {
    return lhs.timestamps() < rhs.timestamps();
  }

  friend ostream &operator<<(ostream &os, TimestampSet const &timestamp_set) {
    bool first = true;
    os << "{";
    for (auto t : timestamp_set.timestamps()) {
      if (first)
        first = false;
      else
        os << ", ";
      os << ISO8601_time(t);
    }
    os << "}";
    return os;
  }

protected:
  set<time_point> m_timestamps;
};

#endif
