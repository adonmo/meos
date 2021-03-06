#include <meos/io/utils.hpp>
#include <meos/types/range/Range.hpp>
#include <sstream>
#include <string>

// For min() and max() to work on MSVC
#ifdef _MSC_VER
#  include <windows.h>
#endif

namespace meos {
using namespace std;

template <typename T> Range<T>::Range() {}

template <typename T>
Range<T>::Range(T const &lower, T const &upper, bool const lower_inc, bool const upper_inc)
    : m_lower(lower), m_upper(upper), m_lower_inc(lower_inc), m_upper_inc(upper_inc) {
  validate();
}

template <typename T> Range<T>::Range(string const &serialized) {
  stringstream ss(serialized);
  Range<T> range;
  ss >> range;
  this->m_lower = range.lower();
  this->m_upper = range.upper();
  this->m_lower_inc = range.lower_inc();
  this->m_upper_inc = range.upper_inc();
  validate();
}

template <typename T> Range<T>::~Range() {}

template <typename T> void Range<T>::validate() const {
  if (this->lower() > this->upper()) {
    throw invalid_argument("The lower bound must be less than or equal to the upper bound");
  }

  if (this->lower() == this->upper()) {
    bool both_inclusive = this->lower_inc() && this->upper_inc();
    if (!both_inclusive) {
      throw invalid_argument("The lower and upper bounds must be inclusive for an instant period");
    }
  }
}

template <typename T> int Range<T>::compare(Range<T> const &other) const {
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

template <typename T> T Range<T>::lower() const { return this->m_lower; }
template <typename T> T Range<T>::upper() const { return this->m_upper; }

template <typename T> bool Range<T>::lower_inc() const { return this->m_lower_inc; }
template <typename T> bool Range<T>::upper_inc() const { return this->m_upper_inc; }

template <typename T> unique_ptr<Range<T>> Range<T>::shift(T const &offset) const {
  return make_unique<Range>(this->lower() + offset, this->upper() + offset, this->lower_inc(),
                            this->upper_inc());
}

template <> unique_ptr<Range<GeomPoint>> Range<GeomPoint>::shift(GeomPoint const &offset) const {
  return make_unique<Range>(this->lower() + offset, this->upper() + offset, this->lower_inc(),
                            this->upper_inc());
}

template <typename T> bool Range<T>::overlap(Range const &p) const {
  auto min_upper = min(this->upper(), p.upper());
  auto max_lower = max(this->lower(), p.lower());
  if (min_upper == max_lower)
    return this->lower() < p.lower() ? this->upper_inc() && p.lower_inc()
                                     : p.upper_inc() && this->lower_inc();
  return min_upper > max_lower;
}

template <typename T> bool Range<T>::contains(T const &t) const {
  return ((this->lower() < t && t < this->upper()) || (this->lower_inc() && this->lower() == t)
          || (this->upper_inc() && this->upper() == t));
}

template <typename T> std::istream &Range<T>::read(std::istream &in) {
  char c;

  c = consume_one_of(in, "[(");
  bool const lower_inc = c == '[';
  auto lower = nextValue<T>(in);
  consume(in, ",");
  auto upper = nextValue<T>(in);
  c = consume_one_of(in, ")]");
  bool const upper_inc = c == ']';

  this->m_lower = lower;
  this->m_upper = upper;
  this->m_lower_inc = lower_inc;
  this->m_upper_inc = upper_inc;

  return in;
}

template <typename T> std::ostream &Range<T>::write(std::ostream &os) const {
  auto opening = this->lower_inc() ? "[" : "(";
  auto closing = this->upper_inc() ? "]" : ")";
  os << opening << this->lower() << ", " << this->upper() << closing;
  return os;
}

template class Range<bool>;
template class Range<int>;
template class Range<float>;
template class Range<string>;
template class Range<GeomPoint>;

}  // namespace meos
