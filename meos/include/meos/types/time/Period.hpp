#ifndef MEOS_TYPES_TIME_PERIOD_HPP
#define MEOS_TYPES_TIME_PERIOD_HPP

#include <ctime>
#include <iomanip>
#include <meos/util/time.hpp>

using namespace std;

class Period {
private:
  time_t const m_lower;
  time_t const m_upper;
  bool const m_lower_inc;
  bool const m_upper_inc;

  int compare(Period const &other) const {
    if (lower() < other.lower())
      return -1;
    else if (lower() > other.lower())
      return 1;
    else if (upper() < other.upper())
      return -1;
    else if (upper() > other.upper())
      return 1;
    else if (lower_inc() && !other.lower_inc())
      return -1;
    else if (!lower_inc() && other.lower_inc())
      return 1;
    else if (upper_inc() && !other.upper_inc())
      return -1;
    else if (!upper_inc() && other.upper_inc())
      return 1;
    return 0;
  }

public:
  Period(time_t const lower, time_t const upper, bool const lower_inc = false,
         bool const upper_inc = false);

  virtual unique_ptr<Period> clone() { return make_unique<Period>(*this); }

  time_t lower() const;
  time_t upper() const;
  bool lower_inc() const;
  bool upper_inc() const;
  time_t timespan() const;
  unique_ptr<Period> shift(time_t const timedelta) const;
  bool overlap(Period const &p) const;
  bool contains_timestamp(time_t const t) const;

  friend bool operator==(Period const &lhs, Period const &rhs) {
    return lhs.compare(rhs) == 0;
  }

  friend bool operator!=(Period const &lhs, Period const &rhs) {
    return lhs.compare(rhs) != 0;
  }

  friend bool operator<(Period const &lhs, Period const &rhs) {
    return lhs.compare(rhs) == -1;
  }

  friend bool operator>(Period const &lhs, Period const &rhs) {
    return rhs < lhs;
  }

  friend bool operator>=(Period const &lhs, Period const &rhs) {
    return !(lhs < rhs);
  }

  friend bool operator<=(Period const &lhs, Period const &rhs) {
    return !(rhs < lhs);
  }

  friend ostream &operator<<(ostream &os, Period const &period) {
    auto opening = period.lower_inc() ? "[" : "(";
    auto closing = period.upper_inc() ? "]" : ")";
    os << opening << ISO8601_time(period.lower()) << ", "
       << ISO8601_time(period.upper()) << closing;
    return os;
  }
};

#endif
