#ifndef MEOS_TYPES_TEMPORAL_TINSTANTFUNCTIONS_HPP
#define MEOS_TYPES_TEMPORAL_TINSTANTFUNCTIONS_HPP

#include <string>

/**
 * This template class uses CRTP pattern and assumes the presence of the
 * following function defined on the TemporalType class:
 *
 *   set<TInstantType> TemporalType::instants() const
 */
template <typename TemporalType, typename TInstantType>
struct TInstantFunctions {
  /**
   * Number of distinct instants.
   */
  int numInstants() const { return this->temporal().instants().size(); };

  /**
   * Start instant.
   */
  TInstantType startInstant() const {
    auto s = this->temporal().instants();
    if (s.size() <= 0) {
      throw "At least one instant expected";
    }
    return *s.begin();
  };

  /**
   * End instant.
   */
  TInstantType endInstant() const {
    auto s = this->temporal().instants();
    if (s.size() <= 0) {
      throw "At least one instant expected";
    }
    return *s.rbegin();
  };

  /**
   * N-th distinct instant.
   */
  TInstantType instantN(int n) const {
    auto s = this->temporal().instants();
    if (s.size() < n) {
      throw "At least " + std::to_string(n) + " instant(s) expected";
    }
    return *next(s.begin(), n);
  };

private:
  TemporalType &temporal() { return static_cast<TemporalType &>(*this); }

  TemporalType const &temporal() const {
    return static_cast<TemporalType const &>(*this);
  }

  // The constructor and friend is to ensure that we are doing proper CRTP
  TInstantFunctions(){};
  friend TemporalType;
};

#endif
