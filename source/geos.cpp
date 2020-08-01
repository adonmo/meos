#include <meos/geos.hpp>

namespace meos {

GEOSContextHandle_t geos_context = NULL;

void init_geos() { geos_context = GEOS_init_r(); }

void finish_geos() { GEOS_finish_r(geos_context); }

}  // namespace meos
