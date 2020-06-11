#include <meos/types/range/Range.hpp>

template <typename T>
Range<T>::Range(T const lower, T const upper, bool const lower_inc,
                bool const upper_inc)
    : m_lower(lower), m_upper(upper), m_lower_inc(lower_inc),
      m_upper_inc(upper_inc) {}

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

template <typename T> bool Range<T>::overlap(Range const &p) const {
  T const o = min(this->upper(), p.upper()) - max(this->lower(), p.lower());
  if (o > 0)
    return true;
  if (o < 0)
    return false;
  return this->lower() < p.lower() ? this->upper_inc() && p.lower_inc()
                                   : p.upper_inc() && this->lower_inc();
};

template <typename T> bool Range<T>::contains(T const t) const {
  return ((this->lower() < t && t < this->upper()) ||
          (this->lower_inc() && this->lower() == t) ||
          (this->upper_inc() && this->upper() == t));
}
