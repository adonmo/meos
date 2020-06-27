#ifndef MEOS_TYPES_TEMPORAL_TEMPORALCOMPARATORS_HPP
#define MEOS_TYPES_TEMPORAL_TEMPORALCOMPARATORS_HPP

#include <string>

/**
 * This template class uses CRTP pattern and assumes the presence of the
 * following function defined on the TemporalType class:
 *
 *   int TemporalType::compare(TemporalType const &other) const
 *
 * where TemporalType == a class extending Temporal
 */
template <typename TemporalType> struct TemporalComparators {
  friend bool operator==(TemporalType const &lhs, TemporalType const &rhs) {
    return lhs.compare(rhs) == 0;
  }

  friend bool operator!=(TemporalType const &lhs, TemporalType const &rhs) {
    return lhs.compare(rhs) != 0;
  }

  friend bool operator<(TemporalType const &lhs, TemporalType const &rhs) {
    return lhs.compare(rhs) == -1;
  }

  friend bool operator>(TemporalType const &lhs, TemporalType const &rhs) {
    return rhs < lhs;
  }

  friend bool operator>=(TemporalType const &lhs, TemporalType const &rhs) {
    return !(lhs < rhs);
  }

  friend bool operator<=(TemporalType const &lhs, TemporalType const &rhs) {
    return !(rhs < lhs);
  }

private:
  // The constructor and friend is to ensure that we are doing proper CRTP
  TemporalComparators(){};
  friend TemporalType;
};

#endif
