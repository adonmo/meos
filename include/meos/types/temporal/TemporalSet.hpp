#ifndef MEOS_TYPES_TEMPORAL_TEMPORALSET_HPP
#define MEOS_TYPES_TEMPORAL_TEMPORALSET_HPP

#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantFunctions.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/types/temporal/TemporalComparators.hpp>
#include <meos/util/serializing.hpp>
#include <set>
#include <string>

using namespace std;

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

template <typename BaseType = float> class TemporalSet
    : public Temporal<BaseType>,
      public TemporalComparators<TemporalSet<BaseType>>,
      public TInstantFunctions<TemporalSet<BaseType>, TInstant<BaseType>, BaseType> {
public:
  TemporalSet();
  TemporalSet(set<TInstant<BaseType>> const &instants);

  /**
   * @brief Set of instants.
   */
  set<TInstant<BaseType>> instants() const;

  /**
   * @brief Set of timestamps.
   */
  set<time_point> timestamps() const override;

protected:
  set<TInstant<BaseType>> m_instants;
};

typedef TemporalSet<bool> TBoolSet;
typedef TemporalSet<int> TIntSet;
typedef TemporalSet<float> TFloatSet;
typedef TemporalSet<string> TTextSet;
typedef TemporalSet<GeomPoint> TGeomPointSet;

#endif
