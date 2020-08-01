#ifndef MEOS_TYPES_TEMPORAL_TINSTANTSET_HPP
#define MEOS_TYPES_TEMPORAL_TINSTANTSET_HPP

#include <meos/types/geom/GeomPoint.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/types/temporal/TemporalSet.hpp>
#include <set>
#include <string>

using namespace std;

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

template <typename BaseType = float> class TInstantSet : public TemporalSet<BaseType> {
public:
  TInstantSet();
  TInstantSet(set<TInstant<BaseType>> const &instants);
  TInstantSet(set<string> const &instants);
  TInstantSet(string const &serialized);

  // Additional constructors for GeomPoint base type to specify SRID
  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TInstantSet(set<TInstant<BaseType>> const &instants, int srid);

  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TInstantSet(set<string> const &instants, int srid);

  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TInstantSet(string const &serialized, int srid);

  int compare(Temporal<BaseType> const &other) const override;

  unique_ptr<TInstantSet<BaseType>> clone() const {
    return std::unique_ptr<TInstantSet<BaseType>>(this->clone_impl());
  }

  TemporalDuration duration() const override { return TemporalDuration::InstantSet; };

  duration_ms timespan() const override;
  set<Range<BaseType>> getValues() const override;
  PeriodSet getTime() const override;
  Period period() const override;
  unique_ptr<TInstantSet<BaseType>> shift(duration_ms const timedelta) const;
  TInstantSet<BaseType> *shift_impl(duration_ms const timedelta) const override;
  bool intersectsTimestamp(time_point const datetime) const override;
  bool intersectsPeriod(Period const period) const override;

  istream &read(istream &in);
  ostream &write(ostream &os) const;

  friend istream &operator>>(istream &in, TInstantSet &instant_set) { return instant_set.read(in); }

  friend ostream &operator<<(ostream &os, TInstantSet<BaseType> const &instant_set) {
    return instant_set.write(os);
  }

private:
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

  TInstantSet<BaseType> *clone_impl() const override { return new TInstantSet<BaseType>(*this); };
};

#endif
