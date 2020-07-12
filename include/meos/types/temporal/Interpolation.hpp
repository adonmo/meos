#ifndef MEOS_TYPES_TEMPORAL_INTERPOLATION_HPP
#define MEOS_TYPES_TEMPORAL_INTERPOLATION_HPP

#include <iosfwd>
#include <string>

using namespace std;

/**
 * Temporal sequences and sequence sets can specify interpolation.
 *
 * Discrete base types like bool, int and text can only have Stepwise
 * interpolation. However other base types like float and geom can have Linear
 * interpolation as well.
 *
 * The following type traits are also provided:
 *   default_interp_v<BaseType> for getting the default Interpolation.
 *   is_discrete_v<BaseType> for checking if a base type is discrete or not.
 */
enum class Interpolation {
  Stepwise,
  Linear,
};

/**
 * Outputs a string representation of the Interpolation to the stream
 */
ostream &operator<<(ostream &os, const Interpolation &i);

/**
 * Helps decide the appropriate interpolation for specified base type
 *
 * Also check default_interp_v<BaseType>, which should be more easy to use
 */
template <typename BaseType> struct default_interp {
  static const Interpolation value = Interpolation::Linear;
};
template <> struct default_interp<bool> {
  static const Interpolation value = Interpolation::Stepwise;
};
template <> struct default_interp<int> {
  static const Interpolation value = Interpolation::Stepwise;
};
template <> struct default_interp<string> {
  static const Interpolation value = Interpolation::Stepwise;
};

/**
 * Shorthand for default_interp<BaseType>::value
 */
template <typename BaseType>
constexpr Interpolation default_interp_v = default_interp<BaseType>::value;

/**
 * Helps find out if a base type is discrete or not
 *
 * Also check is_discrete_v<BaseType>, which should be more easy to use
 */
template <typename BaseType> struct is_discrete {
  static const bool value = false;
};
template <> struct is_discrete<bool> { static const bool value = true; };
template <> struct is_discrete<int> { static const bool value = true; };
template <> struct is_discrete<string> { static const bool value = true; };

/**
 * Shorthand for is_discrete<BaseType>::value
 */
template <typename BaseType>
constexpr bool is_discrete_v = is_discrete<BaseType>::value;

#endif
