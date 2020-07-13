#ifndef MEOS_TYPES_TEMPORAL_TINSTANTSET_HPP
#define MEOS_TYPES_TEMPORAL_TINSTANTSET_HPP

#include <set>

#include <meos/io/utils.hpp>
#include <meos/types/geom/Geometry.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantFunctions.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/types/temporal/TemporalComparators.hpp>
#include <meos/util/serializing.hpp>

using namespace std;

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

template <typename T = float>
class TInstantSet : public Temporal<T>,
                    public TemporalComparators<TInstantSet<T>>,
                    public TInstantFunctions<TInstantSet<T>, TInstant<T>, T> {
public:
  TInstantSet();
  TInstantSet(set<TInstant<T>> const &instants);
  TInstantSet(set<string> const &instants);
  TInstantSet(string const &serialized);

  int compare(Temporal<T> const &other) const override;

  unique_ptr<TInstantSet<T>> clone() const {
    return std::unique_ptr<TInstantSet<T>>(this->clone_impl());
  }

  TemporalDuration duration() const override {
    return TemporalDuration::InstantSet;
  };

  /**
   * Set of instants.
   */
  set<TInstant<T>> instants() const;

  duration_ms timespan() const override;
  set<Range<T>> getValues() const override;
  set<time_point> timestamps() const override;
  PeriodSet getTime() const override;
  Period period() const override;
  unique_ptr<TInstantSet<T>> shift(duration_ms const timedelta) const;
  TInstantSet<T> *shift_impl(duration_ms const timedelta) const override;
  bool intersectsTimestamp(time_point const datetime) const override;
  bool intersectsPeriod(Period const period) const override;

  istream &read(istream &in);
  ostream &write(ostream &os) const;

  friend istream &operator>>(istream &in, TInstantSet &instant_set) {
    return instant_set.read(in);
  }

  friend ostream &operator<<(ostream &os, TInstantSet<T> const &instant_set) {
    return instant_set.write(os);
  }

private:
  set<TInstant<T>> m_instants;

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
