#ifndef MEOS_TESTS_COMMON_MATCHERS_HPP
#define MEOS_TESTS_COMMON_MATCHERS_HPP

#include <set>

namespace Set {
template <typename T>
struct UnorderedEqualsMatcher
    : Catch::Matchers::Impl::MatcherBase<std::set<T>> {
  UnorderedEqualsMatcher(std::set<T> const &target) : m_target(target) {}
  bool match(std::set<T> const &v) const override {
    // Note: This is a reimplementation of std::is_permutation,
    //       because I don't want to include <algorithm> inside the common path
    if (m_target.size() != v.size()) {
      return false;
    }
    return std::is_permutation(m_target.begin(), m_target.end(), v.begin());
  }

  std::string describe() const override {
    return "UnorderedEquals: " + ::Catch::Detail::stringify(m_target);
  }

private:
  std::set<T> const &m_target;
};
} // namespace Set

template <typename T>
Set::UnorderedEqualsMatcher<T> UnorderedEquals(std::set<T> const &target) {
  return Set::UnorderedEqualsMatcher<T>(target);
}

#endif
