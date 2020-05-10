#ifndef MEOS_TYPES_GEOM_GEOMETRY_HPP
#define MEOS_TYPES_GEOM_GEOMETRY_HPP

#include <meos/geos.h>
#include <string>

/**
 * Mostly just a wrapper on top of GEOSGeometry*
 * Main reason we have this is that pybind11 can't work with GEOSGeometry*
 * directly, as it is exposed as an opaque type from the GEOS C API
 */
class Geometry {
public:
  GEOSGeometry *geom = nullptr;

  Geometry(std::string wkt);
  Geometry(double x, double y);
  Geometry(const Geometry &g);
  void operator=(const Geometry &g);
  ~Geometry();

  void point(double x, double y);
  void fromWKT(std::string wkt);
  std::string toWKT() const;

  friend bool operator==(const Geometry &lhs, const Geometry &rhs) {
    return lhs.geom == rhs.geom;
  }

  friend bool operator!=(const Geometry &lhs, const Geometry &rhs) {
    return !(lhs == rhs);
  }

  friend bool operator<(const Geometry &lhs, const Geometry &rhs) {
    // TODO fixme
    return lhs.geom < rhs.geom;
  }

private:
  void free();
};

#endif
