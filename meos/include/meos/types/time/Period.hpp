#ifndef MEOS_TYPES_TIME_PERIOD_HPP
#define MEOS_TYPES_TIME_PERIOD_HPP

#include <ctime>
#include <iomanip>

using namespace std;

class Period {
private:
  time_t m_lower;
  time_t m_upper;
  bool m_lower_inc;
  bool m_upper_inc;

  void validate() const;
  int compare(Period const &other) const;

public:
  Period(time_t const lower, time_t const upper, bool const lower_inc = true,
         bool const upper_inc = false);
  Period(string const lower, string const upper, bool const lower_inc = true,
         bool const upper_inc = false);
  Period(string const serialized);

  virtual unique_ptr<Period> clone() {
    return make_unique<Period>(this->m_lower, this->m_upper, this->m_lower_inc,
                               this->m_upper_inc);
  }

  time_t lower() const;
  time_t upper() const;
  bool lower_inc() const;
  bool upper_inc() const;
  time_t timespan() const;
  unique_ptr<Period> shift(time_t const timedelta) const;
  bool overlap(Period const &p) const;
  bool contains_timestamp(time_t const t) const;

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
