#pragma once

#include <meos/io/utils.hpp>
#include <meos/types/geom/GeomPoint.hpp>
#include <meos/types/temporal/TInstantFunctions.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/types/temporal/TemporalComparators.hpp>
#include <meos/util/serializing.hpp>
#include <string>
#include <type_traits>

namespace meos {

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

/**
 * @brief A value at a particular instant in time.
 */
template <typename BaseType> class TInstant
    : public Temporal<BaseType>,
      public TemporalComparators<TInstant<BaseType>>,
      public TInstantFunctions<TInstant<BaseType>, TInstant<BaseType>, BaseType> {
public:
  // Constructors
  TInstant();
  TInstant(BaseType value, time_point t);
  TInstant(std::pair<BaseType, time_point> p);
  TInstant(std::string const &value, std::string const &t);
  TInstant(std::pair<std::string const, std::string const> p);
  TInstant(std::string const &serialized);

  // Additional constructors for GeomPoint base type to specify SRID
  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TInstant(BaseType value_, time_point t_, int srid);

  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TInstant(std::pair<BaseType, time_point> p, int srid);

  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TInstant(std::string const &value, std::string const &t, int srid);

  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TInstant(std::pair<std::string const, std::string const> p, int srid);

  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TInstant(std::string const &serialized, int srid);

  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TInstant<BaseType> with_srid(int srid) const;

  // Comparision functions
  int compare(Temporal<BaseType> const &other) const override;

  // Accessor functions
  BaseType getValue() const;
  time_point getTimestamp() const;

  std::unique_ptr<TInstant<BaseType>> clone() const {
    return std::unique_ptr<TInstant<BaseType>>(this->clone_impl());
  }

  TemporalDuration duration() const override { return TemporalDuration::Instant; };

  /**
   * @brief Set of instants.
   */
  std::set<TInstant> instants() const;

  duration_ms timespan() const override;
  std::set<Range<BaseType>> getValues() const override;
  std::set<time_point> timestamps() const override;
  PeriodSet getTime() const override;
  Period period() const override;
  std::unique_ptr<TInstant> shift(duration_ms const timedelta) const;
  bool intersectsTimestamp(time_point const datetime) const override;
  bool intersectsPeriod(Period const period) const override;

  // IO functions
  std::istream &read(std::istream &in);
  std::ostream &write(std::ostream &os, bool with_srid = true) const;

  friend std::istream &operator>>(std::istream &in, TInstant &instant) { return instant.read(in); }

  friend std::ostream &operator<<(std::ostream &os, TInstant const &instant) {
    return instant.write(os);
  }

private:
  BaseType value;
  time_point t;

  void validate();

  /**
   * @brief Compares timestamp and value. Does not take SRID into account.
   */
  int compare_internal(Temporal<BaseType> const &other) const;

  TInstant *clone_impl() const override;
  TInstant *shift_impl(duration_ms const timedelta) const override;
};

typedef TInstant<bool> TBoolInst;
typedef TInstant<int> TIntInst;
typedef TInstant<float> TFloatInst;
typedef TInstant<std::string> TTextInst;
typedef TInstant<GeomPoint> TGeomPointInst;

}  // namespace meos
