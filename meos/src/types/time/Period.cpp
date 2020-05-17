#include <meos/types/time/Period.hpp>

Period::Period(std::time_t lower, std::time_t upper, const bool left_open,
               const bool right_open)
    : m_lower(lower), m_upper(upper), m_left_open(left_open),
      m_right_open(right_open) {}

const std::time_t &Period::lower() const { return this->m_lower; }
const std::time_t &Period::upper() const { return this->m_upper; }

const bool &Period::isLeftOpen() const { return this->m_left_open; }
const bool &Period::isRightOpen() const { return this->m_right_open; }
