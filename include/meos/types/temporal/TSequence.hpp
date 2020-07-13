#ifndef MEOS_TYPES_TEMPORAL_TSEQUENCE_HPP
#define MEOS_TYPES_TEMPORAL_TSEQUENCE_HPP

#include <set>

#include <meos/io/utils.hpp>
#include <meos/types/geom/Geometry.hpp>
#include <meos/types/temporal/Interpolation.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantFunctions.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/types/temporal/TemporalComparators.hpp>
#include <meos/util/serializing.hpp>

using namespace std;

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

template <typename T = float>
class TSequence : public Temporal<T>,
                  public TemporalComparators<TSequence<T>>,
                  public TInstantFunctions<TSequence<T>, TInstant<T>, T> {
public:
  TSequence();
  TSequence(set<TInstant<T>> &instants_, bool lower_inc = true,
            bool upper_inc = false,
            Interpolation interpolation = default_interp_v<T>);
  TSequence(set<string> const &instants, bool lower_inc = true,
            bool upper_inc = false,
            Interpolation interpolation = default_interp_v<T>);
  TSequence(string const &serialized);

  int compare(Temporal<T> const &other) const override;

  unique_ptr<TSequence<T>> clone() const {
    return std::unique_ptr<TSequence<T>>(this->clone_impl());
  }

  bool lower_inc() const;
  bool upper_inc() const;

  TemporalDuration duration() const override {
    return TemporalDuration::Sequence;
  };

  /**
   * Set of instants.
   */
  set<TInstant<T>> instants() const;
  Interpolation interpolation() const;
  duration_ms timespan() const override;
  set<Range<T>> getValues() const override;
  set<time_point> timestamps() const override;
  PeriodSet getTime() const override;
  Period period() const override;
  unique_ptr<TSequence<T>> shift(duration_ms const timedelta) const;
  TSequence<T> *shift_impl(duration_ms const timedelta) const override;
  bool intersectsTimestamp(time_point const datetime) const override;
  bool intersectsPeriod(Period const period) const override;

  istream &read(istream &in, bool with_interp = true);
  ostream &write(ostream &os, bool with_interp = true) const;

  friend istream &operator>>(istream &in, TSequence &sequence) {
    return sequence.read(in);
  }

  friend ostream &operator<<(ostream &os, TSequence<T> const &sequence) {
    return sequence.write(os);
  }

private:
  set<TInstant<T>> m_instants;
  bool m_lower_inc;
  bool m_upper_inc;
  Interpolation m_interpolation;

  void validate() const;

  TSequence<T> *clone_impl() const override { return new TSequence<T>(*this); };
};

template class TSequence<bool>;
template class TSequence<int>;
template class TSequence<float>;
template class TSequence<string>;
template class TSequence<Geometry>;

#endif
