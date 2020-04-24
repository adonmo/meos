#ifndef MEOS_TESTS_COMMON_UTILS_HPP
#define MEOS_TESTS_COMMON_UTILS_HPP

#include <iomanip>
#include <set>
#include <vector>

using namespace std;

template <typename T> set<T> unwrap(set<unique_ptr<T>> &sp) {
  set<T> s = {};
  for (const unique_ptr<T> &e : sp) {
    s.insert(*e.get());
  }
  return s;
}

template <typename T> vector<T> unwrap(vector<unique_ptr<T>> &vp) {
  vector<T> v;
  v.reserve(vp.size());
  for (const unique_ptr<T> &e : vp) {
    v.push_back(*e.get());
  }
  return v;
}

vector<string> split(const string &str, const string &delim);

set<string> split_into_set(const string &str, const string &delim);

#endif
