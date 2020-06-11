#ifndef MEOS_TESTS_COMMON_UTILS_HPP
#define MEOS_TESTS_COMMON_UTILS_HPP

#include <iomanip>
#include <set>
#include <vector>

using namespace std;

template <typename T> set<T> unwrap(set<unique_ptr<T>> &sp) {
  set<T> s;
  for (auto const &e : sp) {
    s.insert(*e.get());
  }
  return s;
}

template <typename T> vector<T> unwrap(vector<unique_ptr<T>> &vp) {
  vector<T> v;
  v.reserve(vp.size());
  for (auto const &e : vp) {
    v.push_back(*e.get());
  }
  return v;
}

vector<string> split(string const &str, string const &delim);

set<string> split_into_set(string const &str, string const &delim);

#endif
