#pragma once

#include <iomanip>
#include <meos/io/utils.hpp>
#include <meos/types/geom/GeomPoint.hpp>

namespace meos {

/**
 * @brief %Range of values with either inclusive or exlusive bounds.
 */
template <typename T> class Range {
private:
  T m_lower;
  T m_upper;
  bool m_lower_inc;
  bool m_upper_inc;

  void validate() const;

public:
  Range();
  Range(T const &lower, T const &upper, bool const lower_inc = true, bool const upper_inc = false);
  Range(std::string const &serialized);
  virtual ~Range();

  virtual std::unique_ptr<Range> clone() { return std::make_unique<Range>(*this); }

  int compare(Range const &other) const;

  T lower() const;
  T upper() const;
  bool lower_inc() const;
  bool upper_inc() const;
  std::unique_ptr<Range> shift(T const &offset) const;
  bool overlap(Range const &p) const;
  bool contains(T const &t) const;

  friend bool operator==(Range<T> const &lhs, Range<T> const &rhs) { return lhs.compare(rhs) == 0; }

  friend bool operator!=(Range<T> const &lhs, Range<T> const &rhs) { return lhs.compare(rhs) != 0; }

  friend bool operator<(Range<T> const &lhs, Range<T> const &rhs) { return lhs.compare(rhs) == -1; }

  friend bool operator>(Range<T> const &lhs, Range<T> const &rhs) { return rhs < lhs; }

  friend bool operator>=(Range<T> const &lhs, Range<T> const &rhs) { return !(lhs < rhs); }

  friend bool operator<=(Range<T> const &lhs, Range<T> const &rhs) { return !(rhs < lhs); }

  friend std::istream &operator>>(std::istream &in, Range<T> &range) {
    char c;

    c = consume_one_of(in, "[(");
    bool const lower_inc = c == '[';
    auto lower = nextValue<T>(in);
    consume(in, ",");
    auto upper = nextValue<T>(in);
    c = consume_one_of(in, ")]");
    bool const upper_inc = c == ']';

    range.m_lower = lower;
    range.m_upper = upper;
    range.m_lower_inc = lower_inc;
    range.m_upper_inc = upper_inc;

    return in;
  };

  friend std::ostream &operator<<(std::ostream &os, Range<T> const &range) {
    auto opening = range.lower_inc() ? "[" : "(";
    auto closing = range.upper_inc() ? "]" : ")";
    os << opening << range.lower() << ", " << range.upper() << closing;
    return os;
  }
};

typedef Range<bool> RangeBool;
typedef Range<int> RangeInt;
typedef Range<float> RangeFloat;
typedef Range<std::string> RangeText;
typedef Range<GeomPoint> RangeGeomPoint;

}  // namespace meos
