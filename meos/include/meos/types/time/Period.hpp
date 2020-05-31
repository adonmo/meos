#ifndef MEOS_TYPES_TIME_PERIOD_HPP
#define MEOS_TYPES_TIME_PERIOD_HPP

#include <ctime>
#include <iomanip>

using namespace std;

class Period {
private:
  const time_t m_lower;
  const time_t m_upper;
  const bool m_left_open;
  const bool m_right_open;

  const int compare(const Period &other) const {
    if (lower() < other.lower())
      return -1;
    else if (lower() > other.lower())
      return 1;
    else if (upper() < other.upper())
      return -1;
    else if (upper() > other.upper())
      return 1;
    else if (!isLeftOpen() && other.isLeftOpen())
      return -1;
    else if (isLeftOpen() && !other.isLeftOpen())
      return 1;
    else if (!isRightOpen() && other.isRightOpen())
      return -1;
    else if (isRightOpen() && !other.isRightOpen())
      return 1;
    return 0;
  }

public:
  Period(time_t lower, time_t upper, const bool left_open = false,
         const bool right_open = false);

  virtual unique_ptr<Period> clone() { return make_unique<Period>(*this); }

  const time_t lower() const;
  const time_t upper() const;
  const bool isLeftOpen() const;
  const bool isRightOpen() const;
  const time_t timespan() const;
  const bool overlap(const Period &p) const;
  const bool contains_timestamp(time_t t) const;

  friend bool operator==(const Period &lhs, const Period &rhs) {
    return lhs.compare(rhs) == 0;
  }

  friend bool operator!=(const Period &lhs, const Period &rhs) {
    return lhs.compare(rhs) != 0;
  }

  friend bool operator<(const Period &lhs, const Period &rhs) {
    return lhs.compare(rhs) == -1;
  }
};

#endif
