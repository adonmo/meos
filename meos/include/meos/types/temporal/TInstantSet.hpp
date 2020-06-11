#ifndef MEOS_TYPES_TEMPORAL_TINSTANTSET_HPP
#define MEOS_TYPES_TEMPORAL_TINSTANTSET_HPP

#include <meos/types/geom/Geometry.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantFunctions.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/util/time.hpp>
#include <set>

using namespace std;

template <typename T = float>
class TInstantSet : public Temporal<T>,
                    public TInstantFunctions<TInstantSet<T>, TInstant<T>> {
public:
  set<unique_ptr<TInstant<T>>> m_instants;

  TInstantSet(set<unique_ptr<TInstant<T>>> &instants_);

  TInstantSet(set<TInstant<T>> &instants_);

  unique_ptr<TInstantSet<T>> clone() const {
    return std::unique_ptr<TInstantSet<T>>(this->clone_impl());
  }

  set<TInstant<T>> getInstants() const;

  string duration() const { return "InstantSet"; };

  /**
   * Set of instants.
   */
  set<TInstant<T>> instants() const;

  set<time_t> timestamps() const override;
  PeriodSet getTime() const;
  Period period() const override;
  unique_ptr<TInstantSet<T>> shift(time_t const timedelta) const;
  TInstantSet<T> *shift_impl(time_t const timedelta) const override;
  bool intersectsTimestamp(time_t const datetime) const override;
  bool intersectsPeriod(Period const period) const override;

  friend ostream &operator<<(ostream &os, TInstantSet const &instant_set) {
    bool first = true;
    os << "{";
    for (auto instant : instant_set.getInstants()) {
      if (first)
        first = false;
      else
        os << ", ";
      os << instant;
    }
    os << "}";
    return os;
  }

protected:
  TInstantSet(TInstantSet const &t);

private:
  TInstantSet<T> *clone_impl() const override {
    return new TInstantSet<T>(*this);
  };
};

template class TInstantSet<bool>;
template class TInstantSet<int>;
template class TInstantSet<float>;
template class TInstantSet<string>;
template class TInstantSet<Geometry>;

#endif
