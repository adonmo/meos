#ifndef MEOS_TYPES_TEMPORAL_TSEQUENCE_HPP
#define MEOS_TYPES_TEMPORAL_TSEQUENCE_HPP

#include <meos/io/utils.hpp>
#include <meos/types/geom/Geometry.hpp>
#include <meos/types/temporal/Interpolation.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantFunctions.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/types/temporal/TemporalComparators.hpp>
#include <meos/util/serializing.hpp>
#include <set>

using namespace std;

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

template <typename BaseType = float> class TSequence
    : public Temporal<BaseType>,
      public TemporalComparators<TSequence<BaseType>>,
      public TInstantFunctions<TSequence<BaseType>, TInstant<BaseType>, BaseType> {
public:
  TSequence();
  TSequence(set<TInstant<BaseType>> &instants_, bool lower_inc = true, bool upper_inc = false,
            Interpolation interpolation = default_interp_v<BaseType>);
  TSequence(set<string> const &instants, bool lower_inc = true, bool upper_inc = false,
            Interpolation interpolation = default_interp_v<BaseType>);
  TSequence(string const &serialized);

  // Additional constructors for Geometry base type to specify SRID
  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TSequence(set<TInstant<BaseType>> &instants_, bool lower_inc = true, bool upper_inc = false,
            int srid = SRID_DEFAULT, Interpolation interpolation = default_interp_v<BaseType>);

  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TSequence(set<string> const &instants, bool lower_inc = true, bool upper_inc = false,
            int srid = SRID_DEFAULT, Interpolation interpolation = default_interp_v<BaseType>);

  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TSequence(string const &serialized, int srid);

  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TSequence<BaseType> with_srid(int srid) const;

  TSequence<BaseType> with_interp(Interpolation interpolation) const;

  int compare(Temporal<BaseType> const &other) const override;

  unique_ptr<TSequence<BaseType>> clone() const {
    return std::unique_ptr<TSequence<BaseType>>(this->clone_impl());
  }

  bool lower_inc() const;
  bool upper_inc() const;

  TemporalDuration duration() const override { return TemporalDuration::Sequence; };

  /**
   * Set of instants.
   */
  set<TInstant<BaseType>> instants() const;
  Interpolation interpolation() const;
  duration_ms timespan() const override;
  set<Range<BaseType>> getValues() const override;
  set<time_point> timestamps() const override;
  PeriodSet getTime() const override;
  Period period() const override;
  unique_ptr<TSequence<BaseType>> shift(duration_ms const timedelta) const;
  TSequence<BaseType> *shift_impl(duration_ms const timedelta) const override;
  bool intersectsTimestamp(time_point const datetime) const override;
  bool intersectsPeriod(Period const period) const override;

  istream &read(istream &in, bool with_interp = true, bool with_srid = true);
  ostream &write(ostream &os, bool with_interp = true, bool with_srid = true) const;

  friend istream &operator>>(istream &in, TSequence &sequence) { return sequence.read(in); }

  friend ostream &operator<<(ostream &os, TSequence<BaseType> const &sequence) {
    return sequence.write(os);
  }

  // TODO make this private
  void validate();

private:
  set<TInstant<BaseType>> m_instants;
  bool m_lower_inc;
  bool m_upper_inc;
  Interpolation m_interpolation;

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
   * Use read(istream &in, bool, bool) for reading in general. It internally uses
   * this.
   */
  istream &read_internal(istream &in, bool with_interp = true);

  /**
   * Contains common logic accross all base types.
   * Does not take SRID into account.
   * Use write(ostream &os, bool, bool) for writing in general. It internally uses
   * this.
   */
  ostream &write_internal(ostream &os, bool with_interp = true) const;

  TSequence<BaseType> *clone_impl() const override { return new TSequence<BaseType>(*this); };
};

template class TSequence<bool>;
template class TSequence<int>;
template class TSequence<float>;
template class TSequence<string>;
template class TSequence<Geometry>;

#endif
