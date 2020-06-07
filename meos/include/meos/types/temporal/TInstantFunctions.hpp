#ifndef MEOS_TYPES_TEMPORAL_TINSTANTFUNCTIONS_HPP
#define MEOS_TYPES_TEMPORAL_TINSTANTFUNCTIONS_HPP

#include <string>

template <typename TemporalType, typename TInstantType>
struct TInstantFunctions {
  /**
   * Number of distinct instants.
   */
  int numInstants() const {
    TemporalType const &temporal = static_cast<TemporalType const &>(*this);
    return temporal.instants().size();
  };

  /**
   * Start instant.
   */
  TInstantType startInstant() const {
    TemporalType const &temporal = static_cast<TemporalType const &>(*this);
    auto s = temporal.instants();
    if (s.size() <= 0) {
      throw "At least one instant expected";
    }
    return *s.begin();
  };

  /**
   * End instant.
   */
  TInstantType endInstant() const {
    TemporalType const &temporal = static_cast<TemporalType const &>(*this);
    auto s = temporal.instants();
    if (s.size() <= 0) {
      throw "At least one instant expected";
    }
    return *s.rbegin();
  };

  /**
   * N-th instant.
   */
  TInstantType instantN(int n) const {
    TemporalType const &temporal = static_cast<TemporalType const &>(*this);
    auto s = temporal.instants();
    if (s.size() < n) {
      throw "At least " + std::to_string(n) + " instant(s) expected";
    }
    return *next(s.begin(), n);
  };

private:
  TInstantFunctions(){};
  friend TemporalType;
};

#endif
