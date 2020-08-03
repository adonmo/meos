#pragma once

#include <meos/types/geom/GeomPoint.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/types/temporal/TemporalSet.hpp>
#include <set>
#include <string>

namespace meos {

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

template <typename BaseType = float> class TInstantSet : public TemporalSet<BaseType> {
public:
  TInstantSet();
  TInstantSet(std::set<TInstant<BaseType>> const &instants);
  TInstantSet(std::set<std::string> const &instants);
  TInstantSet(std::string const &serialized);

  // Additional constructors for GeomPoint base type to specify SRID
  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TInstantSet(std::set<TInstant<BaseType>> const &instants, int srid);

  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TInstantSet(std::set<std::string> const &instants, int srid);

  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TInstantSet(std::string const &serialized, int srid);

  int compare(Temporal<BaseType> const &other) const override;

  std::unique_ptr<TInstantSet<BaseType>> clone() const {
    return std::unique_ptr<TInstantSet<BaseType>>(this->clone_impl());
  }

  TemporalDuration duration() const override { return TemporalDuration::InstantSet; };

  duration_ms timespan() const override;
  std::set<Range<BaseType>> getValues() const override;
  PeriodSet getTime() const override;
  Period period() const override;
  std::unique_ptr<TInstantSet<BaseType>> shift(duration_ms const timedelta) const;
  TInstantSet<BaseType> *shift_impl(duration_ms const timedelta) const override;
  bool intersectsTimestamp(time_point const datetime) const override;
  bool intersectsPeriod(Period const period) const override;

  std::istream &read(std::istream &in);
  std::ostream &write(std::ostream &os) const;

  friend std::istream &operator>>(std::istream &in, TInstantSet &instant_set) {
    return instant_set.read(in);
  }

  friend std::ostream &operator<<(std::ostream &os, TInstantSet<BaseType> const &instant_set) {
    return instant_set.write(os);
  }

private:
  void validate();

  /**
   * @brief Only does validations common accross all base types.
   * Use validate() for validating in general. It internally uses this.
   */
  void validate_common();

  /**
   * @brief Compares instants. Does not take SRID into account.
   * Use compare(Temporal<BaseType> const &other) for comparing in general. It
   * internally uses this.
   */
  int compare_internal(Temporal<BaseType> const &other) const;

  /**
   * @brief Contains common logic accross all base types. Does not take SRID into account.
   * Use read(std::istream &in) for reading in general. It internally uses this.
   */
  std::istream &read_internal(std::istream &in);

  /**
   * @brief Contains common logic accross all base types. Does not take SRID into account.
   * Use write(std::ostream &os) for writing in general. It internally uses this.
   */
  std::ostream &write_internal(std::ostream &os) const;

  TInstantSet<BaseType> *clone_impl() const override { return new TInstantSet<BaseType>(*this); };
};

typedef TInstantSet<bool> TBoolInstSet;
typedef TInstantSet<int> TIntInstSet;
typedef TInstantSet<float> TFloatInstSet;
typedef TInstantSet<std::string> TTextInstSet;
typedef TInstantSet<GeomPoint> TGeomPointInstSet;

}  // namespace meos
