#pragma once

/* To avoid accidental use of thread unsafe GEOS functions. */
#define GEOS_USE_ONLY_R_API
#include <geos_c.h>

namespace meos {

extern GEOSContextHandle_t geos_context;
extern void init_geos();
extern void finish_geos();

}  // namespace meos
