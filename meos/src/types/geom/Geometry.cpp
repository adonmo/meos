#include <cstdlib>
#include <meos/types/geom/Geometry.hpp>

Geometry::Geometry(std::string wkt) { fromWKT(wkt); }
Geometry::Geometry(double x, double y) { point(x, y); }
Geometry::Geometry(const Geometry &g) {
  free();
  geom = GEOSGeom_clone_r(geos_context, g.geom);
}
void Geometry::operator=(const Geometry &g) {
  free();
  geom = GEOSGeom_clone_r(geos_context, g.geom);
}
Geometry::~Geometry() { free(); }

void Geometry::point(double x, double y) {
  free();
  geom = GEOSGeom_createPointFromXY_r(geos_context, x, y);
}

void Geometry::fromWKT(std::string wkt) {
  free();
  geom = GEOSGeomFromWKT_r(geos_context, wkt.c_str());
}

std::string Geometry::toWKT() const {
  if (geom != nullptr) {
    GEOSWKTWriter *wktw_ = GEOSWKTWriter_create_r(geos_context);
    GEOSWKTWriter_setTrim_r(geos_context, wktw_, 1);
    GEOSWKTWriter_setRoundingPrecision_r(geos_context, wktw_, 8);
    char *wkt_c = GEOSWKTWriter_write_r(geos_context, wktw_, geom);
    std::string s(wkt_c);
    std::free(wkt_c);
    GEOSWKTWriter_destroy_r(geos_context, wktw_);
    return s;
  }
  throw "Geometry not initiated.";
}

void Geometry::free() {
  if (geom != nullptr) {
    GEOSGeom_destroy_r(geos_context, geom);
    geom = nullptr;
  }
}
