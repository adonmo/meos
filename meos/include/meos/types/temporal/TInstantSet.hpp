#ifndef MEOS_TYPES_TEMPORAL_TINSTANTSET_HPP
#define MEOS_TYPES_TEMPORAL_TINSTANTSET_HPP

#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <set>

using namespace std;

template <typename T> class TInstantSet : public Temporal<T> {
public:
  set<unique_ptr<TInstant<T>>> instants;

  TInstantSet(set<unique_ptr<TInstant<T>>> &instants_);

  TInstantSet(set<TInstant<T>> &instants_);

  set<TInstant<T>> getInstants();

protected:
  TInstantSet(const TInstantSet &t);

private:
};

template class TInstantSet<int>;
template class TInstantSet<float>;

#endif
