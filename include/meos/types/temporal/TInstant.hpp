#ifndef MEOS_TYPES_TEMPORAL_TINSTANT_HPP
#define MEOS_TYPES_TEMPORAL_TINSTANT_HPP

#include <meos/io/utils.hpp>
#include <meos/types/geom/Geometry.hpp>
#include <meos/types/temporal/TInstantFunctions.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/types/temporal/TemporalComparators.hpp>
#include <meos/util/serializing.hpp>
#include <type_traits>

using namespace std;

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

/**
 * Class for representing temporals of instant duration.
 *
 * A temporal instant represents a value at a particular instant in time.
 */
template <typename BaseType> class TInstant
    : public Temporal<BaseType>,
      public TemporalComparators<TInstant<BaseType>>,
      public TInstantFunctions<TInstant<BaseType>, TInstant<BaseType>, BaseType> {
public:
  // Constructors
  TInstant();
  TInstant(BaseType value, time_point t);
  TInstant(pair<BaseType, time_point> p);
  TInstant(string const &value, string const &t);
  TInstant(pair<string const, string const> p);
  TInstant(string const &serialized);

  // Additional constructors for Geometry base type to specify SRID
  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TInstant(BaseType value_, time_point t_, int srid);

  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TInstant(pair<BaseType, time_point> p, int srid);

  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TInstant(string const &value, string const &t, int srid);

  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TInstant(pair<string const, string const> p, int srid);

  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TInstant(string const &serialized, int srid);

  template <typename B = BaseType, typename is_geometry<B>::type * = nullptr>
  TInstant<BaseType> with_srid(int srid) const;

  // Comparision functions
  int compare(Temporal<BaseType> const &other) const override;

  // Accessor functions
  BaseType getValue() const;
  time_point getTimestamp() const;

  unique_ptr<TInstant<BaseType>> clone() const {
    return std::unique_ptr<TInstant<BaseType>>(this->clone_impl());
  }

  TemporalDuration duration() const override { return TemporalDuration::Instant; };

  /**
   * Set of instants.
   */
  set<TInstant> instants() const;

  duration_ms timespan() const override;
  set<Range<BaseType>> getValues() const override;
  set<time_point> timestamps() const override;
  PeriodSet getTime() const override;
  Period period() const override;
  unique_ptr<TInstant> shift(duration_ms const timedelta) const;
  bool intersectsTimestamp(time_point const datetime) const override;
  bool intersectsPeriod(Period const period) const override;

  // IO functions
  istream &read(istream &in);
  ostream &write(ostream &os, bool with_srid = true) const;

  friend istream &operator>>(istream &in, TInstant &instant) { return instant.read(in); }

  friend ostream &operator<<(ostream &os, TInstant const &instant) { return instant.write(os); }

private:
  BaseType value;
  time_point t;

  void validate();

  /**
   * Compares timestamp and value. Does not take SRID into account.
   */
  int compare_internal(Temporal<BaseType> const &other) const;

  TInstant *clone_impl() const override;
  TInstant *shift_impl(duration_ms const timedelta) const override;
};

#endif
