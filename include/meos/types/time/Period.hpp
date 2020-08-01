#pragma once

#include <chrono>
#include <iomanip>
#include <string>

namespace meos {

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
  Period(time_point const lower, time_point const upper, bool const lower_inc = true,
         bool const upper_inc = false);
  Period(std::string const &lower, std::string const &upper, bool const lower_inc = true,
         bool const upper_inc = false);
  Period(std::string const &serialized);

  std::unique_ptr<Period> clone();

  time_point lower() const;
  time_point upper() const;
  bool lower_inc() const;
  bool upper_inc() const;
  duration_ms timespan() const;
  std::unique_ptr<Period> shift(duration_ms const timedelta) const;
  bool overlap(Period const &period) const;
  bool contains_timestamp(time_point const timestamp) const;

  friend bool operator==(Period const &lhs, Period const &rhs);
  friend bool operator!=(Period const &lhs, Period const &rhs);
  friend bool operator<(Period const &lhs, Period const &rhs);
  friend bool operator>(Period const &lhs, Period const &rhs);
  friend bool operator>=(Period const &lhs, Period const &rhs);
  friend bool operator<=(Period const &lhs, Period const &rhs);

  friend std::istream &operator>>(std::istream &in, Period &period);
  friend std::ostream &operator<<(std::ostream &os, Period const &period);
};

}  // namespace meos
