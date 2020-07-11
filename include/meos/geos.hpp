#ifndef MEOS_TYPES_GEOM_GEOS_HPP
#define MEOS_TYPES_GEOM_GEOS_HPP

/* To avoid accidental use of thread unsafe GEOS functions. */
#define GEOS_USE_ONLY_R_API
#include <geos_c.h>

extern GEOSContextHandle_t geos_context;
extern void init_geos();
extern void finish_geos();

#endif
