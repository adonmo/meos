#ifndef MEOS_TYPES_RANGE_RANGE_HPP
#define MEOS_TYPES_RANGE_RANGE_HPP

#include <iomanip>
#include <meos/types/geom/Geometry.hpp>

using namespace std;

template <typename T> class Range {
private:
  T const m_lower;
  T const m_upper;
  bool const m_lower_inc;
  bool const m_upper_inc;

public:
  Range(T const lower, T const upper, bool const lower_inc = true,
        bool const upper_inc = false);

  virtual unique_ptr<Range> clone() { return make_unique<Range>(*this); }

  int compare(Range const &other) const;

  T lower() const;
  T upper() const;
  bool lower_inc() const;
  bool upper_inc() const;
  unique_ptr<Range> shift(T const offset) const;
  bool overlap(Range const &p) const;
  bool contains(T const t) const;

  friend bool operator==(Range<T> const &lhs, Range<T> const &rhs) {
    return lhs.compare(rhs) == 0;
  }

  friend bool operator!=(Range<T> const &lhs, Range<T> const &rhs) {
    return lhs.compare(rhs) != 0;
  }

  friend bool operator<(Range<T> const &lhs, Range<T> const &rhs) {
    return lhs.compare(rhs) == -1;
  }

  friend bool operator>(Range<T> const &lhs, Range<T> const &rhs) {
    return rhs < lhs;
  }

  friend bool operator>=(Range<T> const &lhs, Range<T> const &rhs) {
    return !(lhs < rhs);
  }

  friend bool operator<=(Range<T> const &lhs, Range<T> const &rhs) {
    return !(rhs < lhs);
  }

  friend ostream &operator<<(ostream &os, Range<T> const &range) {
    auto opening = range.lower_inc() ? "[" : "(";
    auto closing = range.upper_inc() ? "]" : ")";
    os << opening << range.lower() << ", " << range.upper() << closing;
    return os;
  }
};

template class Range<bool>;
template class Range<int>;
template class Range<float>;
template class Range<string>;
template class Range<Geometry>;

#endif
