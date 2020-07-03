#ifndef MEOS_TYPES_TEMPORAL_TINSTANTSET_HPP
#define MEOS_TYPES_TEMPORAL_TINSTANTSET_HPP

#include <meos/io/utils.hpp>
#include <meos/types/geom/Geometry.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantFunctions.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/types/temporal/TemporalComparators.hpp>
#include <meos/util/serializing.hpp>
#include <set>

using namespace std;

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

template <typename T = float>
class TInstantSet : public Temporal<T>,
                    public TemporalComparators<TInstantSet<T>>,
                    public TInstantFunctions<TInstantSet<T>, TInstant<T>, T> {
public:
  // TODO FIXME make this private?
  // right now this is public as some tests need it
  set<unique_ptr<TInstant<T>>> m_instants;

  TInstantSet();
  TInstantSet(set<unique_ptr<TInstant<T>>> const &instants);
  TInstantSet(set<TInstant<T>> const &instants);
  TInstantSet(set<string> const &instants);
  TInstantSet(string const &serialized);

  int compare(Temporal<T> const &other) const override;

  unique_ptr<TInstantSet<T>> clone() const {
    return std::unique_ptr<TInstantSet<T>>(this->clone_impl());
  }

  set<TInstant<T>> getInstants() const;

  TemporalDuration duration() const { return TemporalDuration::InstantSet; };

  /**
   * Set of instants.
   */
  set<TInstant<T>> instants() const;

  set<Range<T>> getValues() const override;
  set<time_point> timestamps() const override;
  PeriodSet getTime() const;
  Period period() const override;
  unique_ptr<TInstantSet<T>> shift(duration_ms const timedelta) const;
  TInstantSet<T> *shift_impl(duration_ms const timedelta) const override;
  bool intersectsTimestamp(time_point const datetime) const override;
  bool intersectsPeriod(Period const period) const override;

  friend istream &operator>>(istream &in, TInstantSet<T> &instant_set) {
    char c;

    consume(in, '{');

    set<unique_ptr<TInstant<T>>> s = {};

    TInstant<T> instant;
    in >> instant;
    s.insert(instant.clone());

    while (true) {
      in >> c;
      if (c != ',')
        break;
      in >> instant;
      s.insert(instant.clone());
    }

    if (c != '}') {
      throw invalid_argument("Expected '}'");
    }

    instant_set.m_instants.empty();
    for (auto const &e : s)
      instant_set.m_instants.insert(e->clone());

    return in;
  }

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
