#include <meos/types/time/Period.hpp>

Period::Period(time_t const lower, time_t const upper, bool const lower_inc,
               bool const upper_inc)
    : m_lower(lower), m_upper(upper), m_lower_inc(lower_inc),
      m_upper_inc(upper_inc) {
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

time_t Period::lower() const { return this->m_lower; }
time_t Period::upper() const { return this->m_upper; }

bool Period::lower_inc() const { return this->m_lower_inc; }
bool Period::upper_inc() const { return this->m_upper_inc; }
time_t Period::timespan() const { return this->upper() - this->lower(); }

unique_ptr<Period> Period::shift(time_t const timedelta) const {
  return make_unique<Period>(this->lower() + timedelta,
                             this->upper() + timedelta, this->lower_inc(),
                             this->upper_inc());
}

bool Period::overlap(Period const &p) const {
  time_t const o =
      min(this->upper(), p.upper()) - max(this->lower(), p.lower());
  if (o > 0)
    return true;
  if (o < 0)
    return false;
  return this->lower() < p.lower() ? this->upper_inc() && p.lower_inc()
                                   : p.upper_inc() && this->lower_inc();
};

bool Period::contains_timestamp(time_t const t) const {
  return ((this->lower() < t && t < this->upper()) ||
          (this->lower_inc() && this->lower() == t) ||
          (this->upper_inc() && this->upper() == t));
}
