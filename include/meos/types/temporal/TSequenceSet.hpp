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

template <typename BaseType = float>
class TSequenceSet : public Temporal<BaseType>,
                     public TemporalComparators<TSequenceSet<BaseType>>,
                     public TInstantFunctions<TSequenceSet<BaseType>,
                                              TInstant<BaseType>, BaseType> {
public:
  TSequenceSet();
  TSequenceSet(set<TSequence<BaseType>> const &sequences,
               Interpolation interpolation = default_interp_v<BaseType>);
  TSequenceSet(set<string> const &sequences,
               Interpolation interpolation = default_interp_v<BaseType>);
  TSequenceSet(string const &serialized);

  // Additional constructors for Geometry base type to specify SRID
  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TSequenceSet(set<TSequence<BaseType>> const &sequences, int srid,
               Interpolation interpolation = default_interp_v<BaseType>);

  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TSequenceSet(set<string> const &sequences, int srid,
               Interpolation interpolation = default_interp_v<BaseType>);

  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TSequenceSet(string const &serialized, int srid);

  int compare(Temporal<BaseType> const &other) const override;

  unique_ptr<TSequenceSet<BaseType>> clone() const {
    return std::unique_ptr<TSequenceSet<BaseType>>(this->clone_impl());
  }

  TemporalDuration duration() const override {
    return TemporalDuration::SequenceSet;
  };

  Interpolation interpolation() const;

  /**
   * Set of sequences.
   */
  set<TSequence<BaseType>> sequences() const;

  /**
   * Number of distinct sequences.
   */
  size_t numSequences() const;

  /**
   * Start sequence.
   */
  TSequence<BaseType> startSequence() const;

  /**
   * End sequence.
   */
  TSequence<BaseType> endSequence() const;

  /**
   * N-th distinct sequence.
   */
  TSequence<BaseType> sequenceN(size_t n) const;

  /**
   * Set of instants.
   */
  set<TInstant<BaseType>> instants() const;

  duration_ms timespan() const override;
  set<Range<BaseType>> getValues() const override;
  set<time_point> timestamps() const override;
  PeriodSet getTime() const override;
  Period period() const override;
  unique_ptr<TSequenceSet<BaseType>> shift(duration_ms const timedelta) const;
  TSequenceSet<BaseType> *
  shift_impl(duration_ms const timedelta) const override;
  bool intersectsTimestamp(time_point const datetime) const override;
  bool intersectsPeriod(Period const period) const override;

  istream &read(istream &in);
  ostream &write(ostream &os) const;

  friend istream &operator>>(istream &in, TSequenceSet &sequence_set) {
    return sequence_set.read(in);
  }

  friend ostream &operator<<(ostream &os,
                             TSequenceSet<BaseType> const &sequence_set) {
    return sequence_set.write(os);
  }

private:
  set<TSequence<BaseType>> m_sequences;
  Interpolation m_interpolation;

  void validate();

  /**
   * Only does validations common accross all base types.
   * Use validate() for validating in general. It internally uses this.
   */
  void validate_common();

  /**
   * Compares instants. Does not take SRID into account.
   * Use compare(Temporal<BaseType> const &other) for comparing in general. It
   * internally uses this.
   */
  int compare_internal(Temporal<BaseType> const &other) const;

  /**
   * Contains common logic accross all base types.
   * Does not take SRID into account.
   * Use read(istream &in) for reading in general. It internally uses this.
   */
  istream &read_internal(istream &in);

  /**
   * Contains common logic accross all base types.
   * Does not take SRID into account.
   * Use write(ostream &os) for writing in general. It internally uses this.
   */
  ostream &write_internal(ostream &os) const;

  TSequenceSet<BaseType> *clone_impl() const override {
    return new TSequenceSet<BaseType>(*this);
  };
};

template class TSequenceSet<bool>;
template class TSequenceSet<int>;
template class TSequenceSet<float>;
template class TSequenceSet<string>;
template class TSequenceSet<Geometry>;

#endif
