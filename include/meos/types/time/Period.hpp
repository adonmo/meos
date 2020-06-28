#ifndef MEOS_TYPES_TIME_PERIOD_HPP
#define MEOS_TYPES_TIME_PERIOD_HPP

#include <chrono>
#include <iomanip>

using namespace std;

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

class Period {
private:
  time_point m_lower;
  time_point m_upper;
  bool m_lower_inc;
  bool m_upper_inc;

  void validate() const;
  int compare(Period const &other) const;

public:
  Period();
  Period(time_point const lower, time_point const upper,
         bool const lower_inc = true, bool const upper_inc = false);
  Period(string const &lower, string const &upper, bool const lower_inc = true,
         bool const upper_inc = false);
  Period(string const &serialized);

  unique_ptr<Period> clone();

  time_point lower() const;
  time_point upper() const;
  bool lower_inc() const;
  bool upper_inc() const;
  duration_ms timespan() const;
  unique_ptr<Period> shift(duration_ms const timedelta) const;
  bool overlap(Period const &period) const;
  bool contains_timestamp(time_point const timestamp) const;

  friend bool operator==(Period const &lhs, Period const &rhs);
  friend bool operator!=(Period const &lhs, Period const &rhs);
  friend bool operator<(Period const &lhs, Period const &rhs);
  friend bool operator>(Period const &lhs, Period const &rhs);
  friend bool operator>=(Period const &lhs, Period const &rhs);
  friend bool operator<=(Period const &lhs, Period const &rhs);

  friend istream &operator>>(istream &in, Period &period);
  friend ostream &operator<<(ostream &os, Period const &period);
};

#endif
