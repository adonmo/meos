#pragma once

namespace meos {

/**
 * This template class uses CRTP pattern and assumes the presence of the
 * following function defined on the TemporalType class:
 *
 *   set<TInstantType> TemporalType::instants() const
 *
 * where TemporalType == a class extending Temporal<BaseType>
 *   and TInstantType == TInstant<BaseType>
 */
template <typename TemporalType, typename TInstantType, typename BaseType>
struct TInstantFunctions {
  /**
   * @brief Number of distinct instants.
   */
  size_t numInstants() const { return this->temporal().instants().size(); };

  /**
   * @brief Start instant, irrespective of whether the bounds are inclusive or not.
   */
  TInstantType startInstant() const {
    auto s = this->temporal().instants();
    if (s.size() <= 0) {
      throw "At least one instant expected";
    }
    return *s.begin();
  };

  /**
   * @brief End instant, irrespective of whether the bounds are inclusive or not.
   */
  TInstantType endInstant() const {
    auto s = this->temporal().instants();
    if (s.size() <= 0) {
      throw "At least one instant expected";
    }
    return *s.rbegin();
  };

  /**
   * @brief N-th distinct instant, irrespective of whether the bounds are inclusive or not.
   */
  TInstantType instantN(size_t n) const {
    auto s = this->temporal().instants();
    if (s.size() <= n) {
      throw "At least " + std::to_string(n) + " instant(s) expected";
    }
    return *next(s.begin(), n);
  };

  /**
   * @brief Start value, irrespective of whether the bounds are inclusive or not.
   */
  BaseType startValue() const {
    auto s = this->temporal().instants();
    if (s.size() <= 0) {
      throw "At least one instant expected";
    }
    return this->temporal().startInstant().getValue();
  }

  /**
   * @brief End value, irrespective of whether the bounds are inclusive or not.
   */
  BaseType endValue() const {
    auto s = this->temporal().instants();
    if (s.size() <= 0) {
      throw "At least one instant expected";
    }
    return this->temporal().endInstant().getValue();
  }

  /**
   * @brief N-th value, irrespective of whether the bounds are inclusive or not.
   */
  BaseType valueN(size_t n) const {
    auto s = this->temporal().instants();
    if (s.size() <= n) {
      throw "At least " + std::to_string(n) + " instant(s) expected";
    }
    return this->temporal().instantN(n).getValue();
  }

private:
  TemporalType &temporal() { return static_cast<TemporalType &>(*this); }

  TemporalType const &temporal() const { return static_cast<TemporalType const &>(*this); }

  // The constructor and friend is to ensure that we are doing proper CRTP
  TInstantFunctions(){};
  friend TemporalType;
};

}  // namespace meos
