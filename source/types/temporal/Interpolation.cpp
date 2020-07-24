#include <meos/types/temporal/Interpolation.hpp>
#include <ostream>
#include <string>

ostream &operator<<(ostream &os, const Interpolation &i) {
  if (i == Interpolation::Stepwise) {
    os << "Stepwise";
  } else if (i == Interpolation::Linear) {
    os << "Linear";
  } else {
    // Technically we'd never reach here
    throw invalid_argument("Unsupported interpolation specified: " + to_string(int(i)));
  }
  return os;
}
