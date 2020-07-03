#include <meos/types/range/Range.hpp>

template <typename T>
Range<T>::Range(T const lower, T const upper, bool const lower_inc,
                bool const upper_inc)
    : m_lower(lower), m_upper(upper), m_lower_inc(lower_inc),
      m_upper_inc(upper_inc) {}

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

template <typename T> bool Range<T>::lower_inc() const {
  return this->m_lower_inc;
}
template <typename T> bool Range<T>::upper_inc() const {
  return this->m_upper_inc;
}

template <typename T>
unique_ptr<Range<T>> Range<T>::shift(T const offset) const {
  return make_unique<Range>(this->lower() + offset, this->upper() + offset,
                            this->lower_inc(), this->upper_inc());
}

template <>
unique_ptr<Range<Geometry>>
Range<Geometry>::shift(Geometry const offset) const {
  return make_unique<Range>(this->lower() + offset, this->upper() + offset,
                            this->lower_inc(), this->upper_inc());
}

template <typename T> bool Range<T>::overlap(Range const &p) const {
  auto min_upper = min(this->upper(), p.upper());
  auto max_lower = max(this->lower(), p.lower());
  if (min_upper == max_lower)
    return this->lower() < p.lower() ? this->upper_inc() && p.lower_inc()
                                     : p.upper_inc() && this->lower_inc();
  return min_upper > max_lower;
}

template <typename T> bool Range<T>::contains(T const t) const {
  return ((this->lower() < t && t < this->upper()) ||
          (this->lower_inc() && this->lower() == t) ||
          (this->upper_inc() && this->upper() == t));
}
