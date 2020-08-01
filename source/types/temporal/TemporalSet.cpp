#include <algorithm>
#include <iomanip>
#include <meos/types/temporal/TemporalSet.hpp>
#include <sstream>
#include <string>

namespace meos {
using namespace std;

template <typename BaseType> TemporalSet<BaseType>::TemporalSet() {}

template <typename BaseType>
TemporalSet<BaseType>::TemporalSet(set<TInstant<BaseType>> const &instants)
    : m_instants(instants) {}

template <typename BaseType> set<TInstant<BaseType>> TemporalSet<BaseType>::instants() const {
  return this->m_instants;
}

template <typename BaseType> set<time_point> TemporalSet<BaseType>::timestamps() const {
  set<time_point> s;
  for (auto const &e : this->m_instants) {
    s.insert(e.getTimestamp());
  }
  return s;
}

template class TemporalSet<bool>;
template class TemporalSet<int>;
template class TemporalSet<float>;
template class TemporalSet<string>;
template class TemporalSet<GeomPoint>;

}  // namespace meos
