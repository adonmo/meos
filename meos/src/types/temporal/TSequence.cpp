#include <iomanip>
#include <meos/types/temporal/TSequence.hpp>

template <typename T>
TSequence<T>::TSequence(vector<unique_ptr<TInstant<T>>> &instants_,
                        bool left_open_, bool right_open_)
    : left_open(left_open_), right_open(right_open_) {
  instants.reserve(instants_.size());
  for (const auto &e : instants_)
    instants.push_back(e->clone());
}

template <typename T>
TSequence<T>::TSequence(vector<TInstant<T>> &instants_, bool left_open_,
                        bool right_open_)
    : left_open(left_open_), right_open(right_open_) {
  instants.reserve(instants_.size());
  for (auto e : instants_)
    instants.push_back(e.clone());
}

template <typename T>
TSequence<T>::TSequence(const TSequence &t)
    : left_open(t.left_open), right_open(t.right_open) {
  instants.reserve(t.instants.size());
  for (const auto &e : t.instants)
    instants.push_back(e->clone());
}

template <typename T> vector<TInstant<T>> TSequence<T>::getInstants() {
  vector<TInstant<T>> s = {};
  for (const unique_ptr<TInstant<T>> &e : instants) {
    s.push_back(*e.get());
  }
  return s;
}
