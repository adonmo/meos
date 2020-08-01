#pragma once

#include <meos/types/geom/GeomPoint.hpp>
#include <meos/types/temporal/Interpolation.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/types/temporal/TemporalSet.hpp>
#include <set>
#include <string>

namespace meos {

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

template <typename BaseType = float> class TSequence : public TemporalSet<BaseType> {
public:
  TSequence();
  TSequence(std::set<TInstant<BaseType>> &instants_, bool lower_inc = true, bool upper_inc = false,
            Interpolation interpolation = default_interp_v<BaseType>);
  TSequence(std::set<std::string> const &instants, bool lower_inc = true, bool upper_inc = false,
            Interpolation interpolation = default_interp_v<BaseType>);
  TSequence(std::string const &serialized);

  // Additional constructors for GeomPoint base type to specify SRID
  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TSequence(std::set<TInstant<BaseType>> &instants_, bool lower_inc = true, bool upper_inc = false,
            int srid = SRID_DEFAULT, Interpolation interpolation = default_interp_v<BaseType>);

  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TSequence(std::set<std::string> const &instants, bool lower_inc = true, bool upper_inc = false,
            int srid = SRID_DEFAULT, Interpolation interpolation = default_interp_v<BaseType>);

  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TSequence(std::string const &serialized, int srid);

  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TSequence<BaseType> with_srid(int srid) const;

  TSequence<BaseType> with_interp(Interpolation interpolation) const;

  int compare(Temporal<BaseType> const &other) const override;

  std::unique_ptr<TSequence<BaseType>> clone() const {
    return std::unique_ptr<TSequence<BaseType>>(this->clone_impl());
  }

  bool lower_inc() const;
  bool upper_inc() const;

  TemporalDuration duration() const override { return TemporalDuration::Sequence; };

  Interpolation interpolation() const;
  duration_ms timespan() const override;
  std::set<Range<BaseType>> getValues() const override;
  PeriodSet getTime() const override;
  Period period() const override;
  std::unique_ptr<TSequence<BaseType>> shift(duration_ms const timedelta) const;
  TSequence<BaseType> *shift_impl(duration_ms const timedelta) const override;
  bool intersectsTimestamp(time_point const datetime) const override;
  bool intersectsPeriod(Period const period) const override;

  std::istream &read(std::istream &in, bool with_interp = true, bool with_srid = true);
  std::ostream &write(std::ostream &os, bool with_interp = true, bool with_srid = true) const;

  friend std::istream &operator>>(std::istream &in, TSequence &sequence) {
    return sequence.read(in);
  }

  friend std::ostream &operator<<(std::ostream &os, TSequence<BaseType> const &sequence) {
    return sequence.write(os);
  }

  // TODO make this private
  void validate();

private:
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
   * Use read(std::istream &in, bool, bool) for reading in general. It internally uses
   * this.
   */
  std::istream &read_internal(std::istream &in, bool with_interp = true);

  /**
   * Contains common logic accross all base types.
   * Does not take SRID into account.
   * Use write(std::ostream &os, bool, bool) for writing in general. It internally uses
   * this.
   */
  std::ostream &write_internal(std::ostream &os, bool with_interp = true) const;

  TSequence<BaseType> *clone_impl() const override { return new TSequence<BaseType>(*this); };
};

typedef TSequence<bool> TBoolSeq;
typedef TSequence<int> TIntSeq;
typedef TSequence<float> TFloatSeq;
typedef TSequence<std::string> TTextSeq;
typedef TSequence<GeomPoint> TGeomPointSeq;

}  // namespace meos
