#ifndef MEOS_TYPES_TIME_TIMESTAMPSET_HPP
#define MEOS_TYPES_TIME_TIMESTAMPSET_HPP

#include <iomanip>
#include <iterator>
#include <meos/types/time/Period.hpp>
#include <meos/util/time.hpp>
#include <set>

using namespace std;

class TimestampSet {
public:
  TimestampSet(set<time_t> &timestamps_);

  TimestampSet(TimestampSet const &t);

  set<Period> periods() const;
  Period period() const;
  int numPeriods() const;
  Period startPeriod() const;
  Period endPeriod() const;
  Period periodN(int n) const;

  time_t timespan() const;
  unique_ptr<TimestampSet> shift(time_t const timedelta) const;

  set<time_t> timestamps() const;
  int numTimestamps() const;
  time_t startTimestamp() const;
  time_t endTimestamp() const;
  time_t timestampN(int n) const;

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
  set<time_t> m_timestamps;
};

#endif
