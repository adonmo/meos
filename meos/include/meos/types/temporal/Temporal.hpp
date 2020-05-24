#ifndef MEOS_TYPES_TEMPORAL_TEMPORAL_HPP
#define MEOS_TYPES_TEMPORAL_TEMPORAL_HPP

#include <meos/types/geom/Geometry.hpp>

using namespace std;

template <typename T = float> class Temporal {
private:
public:
  Temporal();
};

template class Temporal<bool>;
template class Temporal<int>;
template class Temporal<float>;
template class Temporal<string>;
template class Temporal<Geometry>;

#endif
