#ifndef MEOS_TYPES_GEOM_GEOS_H
#define MEOS_TYPES_GEOM_GEOS_H

/* To avoid accidental use of thread unsafe GEOS functions. */
#define GEOS_USE_ONLY_R_API
#include <geos_c.h>

#ifdef __cplusplus
extern "C" {
#endif

extern GEOSContextHandle_t geos_context;
extern void init_geos();
extern void finish_geos();

#ifdef __cplusplus
}
#endif

#endif
