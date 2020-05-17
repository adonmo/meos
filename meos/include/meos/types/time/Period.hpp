#ifndef MEOS_TYPES_TIME_PERIOD_HPP
#define MEOS_TYPES_TIME_PERIOD_HPP

#include <ctime>

using namespace std;

class Period {
private:
  time_t m_lower;
  time_t m_upper;
  bool m_left_open;
  bool m_right_open;

public:
  Period(time_t lower, time_t upper, const bool left_open = false,
         const bool right_open = false);
  time_t &lower();
  time_t &upper();
  bool &isLeftOpen();
  bool &isRightOpen();
};

#endif
