#include <meos/types/time/Period.hpp>

Period::Period(std::time_t lower, std::time_t upper, const bool lower_inc,
               const bool upper_inc)
    : m_lower(lower), m_upper(upper), m_lower_inc(lower_inc),
      m_upper_inc(upper_inc) {}

const std::time_t Period::lower() const { return this->m_lower; }
const std::time_t Period::upper() const { return this->m_upper; }

const bool Period::lower_inc() const { return this->m_lower_inc; }
const bool Period::upper_inc() const { return this->m_upper_inc; }
const time_t Period::timespan() const { return this->upper() - this->lower(); }

unique_ptr<Period> Period::shift(const time_t timedelta) const {
  return make_unique<Period>(this->lower() + timedelta,
                             this->upper() + timedelta, this->lower_inc(),
                             this->upper_inc());
}

const bool Period::overlap(const Period &p) const {
  const time_t o =
      min(this->upper(), p.upper()) - max(this->lower(), p.lower());
  if (o > 0)
    return true;
  if (o < 0)
    return false;
  return this->lower() < p.lower() ? this->upper_inc() && p.lower_inc()
                                   : p.upper_inc() && this->lower_inc();
};

const bool Period::contains_timestamp(const time_t t) const {
  return ((this->lower() < t && t < this->upper()) ||
          (this->lower_inc() && this->lower() == t) ||
          (this->upper_inc() && this->upper() == t));
}
