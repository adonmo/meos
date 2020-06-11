#ifndef MEOS_TYPES_RANGE_RANGE_HPP
#define MEOS_TYPES_RANGE_RANGE_HPP

#include <iomanip>

using namespace std;

template <typename T> class Range {
private:
  T const m_lower;
  T const m_upper;
  bool const m_lower_inc;
  bool const m_upper_inc;

  int compare(Range const &other) const {
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
  Range(T const lower, T const upper, bool const lower_inc = false,
        bool const upper_inc = false);

  virtual unique_ptr<Range> clone() { return make_unique<Range>(*this); }

  T lower() const;
  T upper() const;
  bool lower_inc() const;
  bool upper_inc() const;
  unique_ptr<Range> shift(T const offset) const;
  bool overlap(Range const &p) const;
  bool contains(T const t) const;

  friend bool operator==(Range const &lhs, Range const &rhs) {
    return lhs.compare(rhs) == 0;
  }

  friend bool operator!=(Range const &lhs, Range const &rhs) {
    return lhs.compare(rhs) != 0;
  }

  friend bool operator<(Range const &lhs, Range const &rhs) {
    return lhs.compare(rhs) == -1;
  }

  friend ostream &operator<<(ostream &os, Range const &period) {
    auto opening = period.lower_inc() ? "[" : "(";
    auto closing = period.upper_inc() ? "]" : ")";
    os << opening << period.lower() << ", " << period.upper() << closing;
    return os;
  }
};

template class Range<int>;
template class Range<float>;

#endif
