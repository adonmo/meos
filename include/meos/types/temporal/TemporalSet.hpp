#pragma once

#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantFunctions.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/types/temporal/TemporalComparators.hpp>
#include <meos/util/serializing.hpp>
#include <set>
#include <string>

namespace meos {

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

/**
 * @brief Base class for TInstantSet and TSequence.
 */
template <typename BaseType = float> class TemporalSet
    : public Temporal<BaseType>,
      public TemporalComparators<TemporalSet<BaseType>>,
      public TInstantFunctions<TemporalSet<BaseType>, TInstant<BaseType>, BaseType> {
public:
  TemporalSet();
  TemporalSet(std::set<TInstant<BaseType>> const &instants);

  /**
   * @brief Set of instants.
   */
  std::set<TInstant<BaseType>> instants() const;

  /**
   * @brief Set of timestamps.
   */
  std::set<time_point> timestamps() const override;

protected:
  std::set<TInstant<BaseType>> m_instants;
};

typedef TemporalSet<bool> TBoolSet;
typedef TemporalSet<int> TIntSet;
typedef TemporalSet<float> TFloatSet;
typedef TemporalSet<std::string> TTextSet;
typedef TemporalSet<GeomPoint> TGeomPointSet;

}  // namespace meos
