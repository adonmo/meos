#ifndef MEOS_TYPES_TIME_PERIOD_HPP
#define MEOS_TYPES_TIME_PERIOD_HPP

#include <ctime>
#include <iomanip>
#include <meos/util/time.hpp>

using namespace std;

class Period {
private:
  const time_t m_lower;
  const time_t m_upper;
  const bool m_lower_inc;
  const bool m_upper_inc;

  const int compare(const Period &other) const {
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
  Period(const time_t lower, const time_t upper, const bool lower_inc = false,
         const bool upper_inc = false);

  virtual unique_ptr<Period> clone() { return make_unique<Period>(*this); }

  const time_t lower() const;
  const time_t upper() const;
  const bool lower_inc() const;
  const bool upper_inc() const;
  const time_t timespan() const;
  unique_ptr<Period> shift(const time_t timedelta) const;
  const bool overlap(const Period &p) const;
  const bool contains_timestamp(const time_t t) const;

  friend bool operator==(const Period &lhs, const Period &rhs) {
    return lhs.compare(rhs) == 0;
  }

  friend bool operator!=(const Period &lhs, const Period &rhs) {
    return lhs.compare(rhs) != 0;
  }

  friend bool operator<(const Period &lhs, const Period &rhs) {
    return lhs.compare(rhs) == -1;
  }

  friend ostream &operator<<(ostream &os, const Period &period) {
    auto opening = period.lower_inc() ? "[" : "(";
    auto closing = period.upper_inc() ? "]" : ")";
    os << opening << ISO8601_time(period.lower()) << ", "
       << ISO8601_time(period.upper()) << closing;
    return os;
  }
};

#endif
