#include <iostream>

#include <meos/types/temporal/Interpolation.hpp>

ostream &operator<<(ostream &os, const Interpolation &i) {
  if (i == Interpolation::Stepwise) {
    os << "Stepwise";
  } else if (i == Interpolation::Linear) {
    os << "Linear";
  } else {
    // Technically we'd never reach here
    throw invalid_argument("Unsupported interpolation specified: " + int(i));
  }
  return os;
}
