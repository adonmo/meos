#ifndef MEOS_TYPES_TEMPORAL_TSEQUENCESET_HPP
#define MEOS_TYPES_TEMPORAL_TSEQUENCESET_HPP

#include <set>

#include <meos/io/utils.hpp>
#include <meos/types/geom/Geometry.hpp>
#include <meos/types/temporal/Interpolation.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantFunctions.hpp>
#include <meos/types/temporal/TSequence.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/types/temporal/TemporalComparators.hpp>
#include <meos/util/serializing.hpp>

using namespace std;

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

template <typename T = float>
class TSequenceSet : public Temporal<T>,
                     public TemporalComparators<TSequenceSet<T>>,
                     public TInstantFunctions<TSequenceSet<T>, TInstant<T>, T> {
public:
  TSequenceSet();
  TSequenceSet(set<TSequence<T>> const &sequences,
               Interpolation interpolation = default_interp_v<T>);
  TSequenceSet(set<string> const &sequences,
               Interpolation interpolation = default_interp_v<T>);
  TSequenceSet(string const &serialized);

  int compare(Temporal<T> const &other) const override;

  unique_ptr<TSequenceSet<T>> clone() const {
    return std::unique_ptr<TSequenceSet<T>>(this->clone_impl());
  }

  TemporalDuration duration() const override {
    return TemporalDuration::SequenceSet;
  };

  Interpolation interpolation() const;

  /**
   * Set of sequences.
   */
  set<TSequence<T>> sequences() const;

  /**
   * Number of distinct sequences.
   */
  size_t numSequences() const;

  /**
   * Start sequence.
   */
  TSequence<T> startSequence() const;

  /**
   * End sequence.
   */
  TSequence<T> endSequence() const;

  /**
   * N-th distinct sequence.
   */
  TSequence<T> sequenceN(size_t n) const;

  /**
   * Set of instants.
   */
  set<TInstant<T>> instants() const;

  duration_ms timespan() const override;
  set<Range<T>> getValues() const override;
  set<time_point> timestamps() const override;
  PeriodSet getTime() const override;
  Period period() const override;
  unique_ptr<TSequenceSet<T>> shift(duration_ms const timedelta) const;
  TSequenceSet<T> *shift_impl(duration_ms const timedelta) const override;
  bool intersectsTimestamp(time_point const datetime) const override;
  bool intersectsPeriod(Period const period) const override;

  istream &read(istream &in);
  ostream &write(ostream &os) const;

  friend istream &operator>>(istream &in, TSequenceSet &sequence_set) {
    return sequence_set.read(in);
  }

  friend ostream &operator<<(ostream &os, TSequenceSet<T> const &sequence_set) {
    return sequence_set.write(os);
  }

private:
  set<TSequence<T>> m_sequences;
  Interpolation m_interpolation;

  void validate() const;

  TSequenceSet<T> *clone_impl() const override {
    return new TSequenceSet<T>(*this);
  };
};

template class TSequenceSet<bool>;
template class TSequenceSet<int>;
template class TSequenceSet<float>;
template class TSequenceSet<string>;
template class TSequenceSet<Geometry>;

#endif
