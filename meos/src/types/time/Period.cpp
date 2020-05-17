#include <meos/types/time/Period.hpp>

Period::Period(std::time_t lower, std::time_t upper, const bool left_open,
               const bool right_open)
    : m_lower(lower), m_upper(upper), m_left_open(left_open),
      m_right_open(right_open) {}

std::time_t &Period::lower() { return this->m_lower; }
std::time_t &Period::upper() { return this->m_upper; }

bool &Period::isLeftOpen() { return this->m_left_open; }
bool &Period::isRightOpen() { return this->m_right_open; }
