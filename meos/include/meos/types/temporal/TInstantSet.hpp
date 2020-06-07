#ifndef MEOS_TYPES_TEMPORAL_TINSTANTSET_HPP
#define MEOS_TYPES_TEMPORAL_TINSTANTSET_HPP

#include <meos/types/geom/Geometry.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <set>

using namespace std;

template <typename T = float> class TInstantSet : public Temporal<T> {
public:
  set<unique_ptr<TInstant<T>>> instants;

  TInstantSet(set<unique_ptr<TInstant<T>>> &instants_);

  TInstantSet(set<TInstant<T>> &instants_);

  set<TInstant<T>> getInstants() const;

  set<time_t> timestamps() const override;
  const Period period() const override;

protected:
  TInstantSet(const TInstantSet &t);

private:
};

template class TInstantSet<bool>;
template class TInstantSet<int>;
template class TInstantSet<float>;
template class TInstantSet<string>;
template class TInstantSet<Geometry>;

#endif
