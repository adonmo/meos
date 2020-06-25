#include <meos/io/utils.hpp>
#include <meos/types/time/Period.hpp>
#include <meos/util/time.hpp>

Period::Period(time_point const lower, time_point const upper,
               bool const lower_inc, bool const upper_inc)
    : m_lower(lower), m_upper(upper), m_lower_inc(lower_inc),
      m_upper_inc(upper_inc) {
  validate();
}

Period::Period(string const lower, string const upper, bool const lower_inc,
               bool const upper_inc)
    : m_lower_inc(lower_inc), m_upper_inc(upper_inc) {
  stringstream lss(lower);
  this->m_lower = std::chrono::system_clock::from_time_t(nextTime(lss) / 1000L);
  stringstream uss(upper);
  this->m_upper = std::chrono::system_clock::from_time_t(nextTime(uss) / 1000L);
  validate();
}

Period::Period(string const serialized) {
  stringstream ss(serialized);
  Period period(std::chrono::system_clock::from_time_t(0),
                std::chrono::system_clock::from_time_t(1));
  ss >> period;
  this->m_lower = period.lower();
  this->m_upper = period.upper();
  this->m_lower_inc = period.lower_inc();
  this->m_upper_inc = period.upper_inc();
  validate();
}

void Period::validate() const {
  if (this->lower() > this->upper()) {
    throw invalid_argument(
        "The lower bound must be less than or equal to the upper bound");
  }

  if (this->lower() == this->upper()) {
    bool both_inclusive = this->lower_inc() && this->upper_inc();
    if (!both_inclusive) {
      throw invalid_argument(
          "The lower and upper bounds must be inclusive for an instant period");
    }
  }
}

time_point Period::lower() const { return this->m_lower; }
time_point Period::upper() const { return this->m_upper; }

bool Period::lower_inc() const { return this->m_lower_inc; }
bool Period::upper_inc() const { return this->m_upper_inc; }
duration_ms Period::timespan() const {
  return chrono::duration_cast<chrono::milliseconds>(this->upper() -
                                                     this->lower());
}

unique_ptr<Period> Period::shift(duration_ms const timedelta) const {
  return make_unique<Period>(this->lower() + timedelta,
                             this->upper() + timedelta, this->lower_inc(),
                             this->upper_inc());
}

bool Period::overlap(Period const &p) const {
  duration_ms const o = chrono::duration_cast<chrono::milliseconds>(
      min(this->upper(), p.upper()) - max(this->lower(), p.lower()));
  if (o.count() > 0)
    return true;
  if (o.count() < 0)
    return false;
  return this->lower() < p.lower() ? this->upper_inc() && p.lower_inc()
                                   : p.upper_inc() && this->lower_inc();
};

bool Period::contains_timestamp(time_point const t) const {
  return ((this->lower() < t && t < this->upper()) ||
          (this->lower_inc() && this->lower() == t) ||
          (this->upper_inc() && this->upper() == t));
}

int Period::compare(Period const &other) const {
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

bool operator==(Period const &lhs, Period const &rhs) {
  return lhs.compare(rhs) == 0;
}

bool operator!=(Period const &lhs, Period const &rhs) {
  return lhs.compare(rhs) != 0;
}

bool operator<(Period const &lhs, Period const &rhs) {
  return lhs.compare(rhs) == -1;
}

bool operator>(Period const &lhs, Period const &rhs) { return rhs < lhs; }

bool operator>=(Period const &lhs, Period const &rhs) { return !(lhs < rhs); }

bool operator<=(Period const &lhs, Period const &rhs) { return !(rhs < lhs); }

istream &operator>>(istream &in, Period &period) {
  char c;

  in >> c;
  if (c != '[' && c != '(') {
    throw invalid_argument("Expected either a '[' or '('");
  }
  bool const lower_inc = c == '[';

  auto lower = std::chrono::system_clock::from_time_t(nextTime(in) / 1000L);

  in >> c;
  if (c != ',') {
    throw invalid_argument("Expected a ','");
  }

  auto upper = std::chrono::system_clock::from_time_t(nextTime(in) / 1000L);

  in >> c;
  if (c != ']' && c != ')') {
    throw invalid_argument("Expected either a ']' or ')'");
  }
  bool const upper_inc = c == ']';

  period.m_lower = lower;
  period.m_upper = upper;
  period.m_lower_inc = lower_inc;
  period.m_upper_inc = upper_inc;

  return in;
}

ostream &operator<<(ostream &os, Period const &period) {
  auto opening = period.lower_inc() ? "[" : "(";
  auto closing = period.upper_inc() ? "]" : ")";
  os << opening
     << ISO8601_time(std::chrono::system_clock::to_time_t(period.lower()) *
                     1000)
     << ", "
     << ISO8601_time(std::chrono::system_clock::to_time_t(period.upper()) *
                     1000)
     << closing;
  return os;
}
