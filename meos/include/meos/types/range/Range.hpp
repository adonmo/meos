#ifndef MEOS_TYPES_RANGE_RANGE_HPP
#define MEOS_TYPES_RANGE_RANGE_HPP

#include <iomanip>

using namespace std;

template <typename T> class Range {
private:
  const T m_lower;
  const T m_upper;
  const bool m_lower_inc;
  const bool m_upper_inc;

  int compare(const Range &other) const {
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

public:
  Range(const T lower, const T upper, const bool lower_inc = false,
        const bool upper_inc = false);

  virtual unique_ptr<Range> clone() { return make_unique<Range>(*this); }

  T lower() const;
  T upper() const;
  bool lower_inc() const;
  bool upper_inc() const;
  unique_ptr<Range> shift(const T offset) const;
  bool overlap(const Range &p) const;
  bool contains(const T t) const;

  friend bool operator==(const Range &lhs, const Range &rhs) {
    return lhs.compare(rhs) == 0;
  }

  friend bool operator!=(const Range &lhs, const Range &rhs) {
    return lhs.compare(rhs) != 0;
  }

  friend bool operator<(const Range &lhs, const Range &rhs) {
    return lhs.compare(rhs) == -1;
  }

  friend ostream &operator<<(ostream &os, const Range &period) {
    auto opening = period.lower_inc() ? "[" : "(";
    auto closing = period.upper_inc() ? "]" : ")";
    os << opening << period.lower() << ", " << period.upper() << closing;
    return os;
  }
};

template class Range<int>;
template class Range<float>;

#endif
