#ifndef MEOS_TYPES_GEOM_GEOMETRY_HPP
#define MEOS_TYPES_GEOM_GEOMETRY_HPP

#include <iosfwd>
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
  Geometry(Geometry const &g);
  void operator=(Geometry const &g);
  ~Geometry();

  void point(double x, double y);
  void fromWKT(std::string wkt);
  std::string toWKT() const;

  friend bool operator==(Geometry const &lhs, Geometry const &rhs) {
    return lhs.geom == rhs.geom;
  }

  friend bool operator!=(Geometry const &lhs, Geometry const &rhs) {
    return !(lhs == rhs);
  }

  friend bool operator<(Geometry const &lhs, Geometry const &rhs) {
    // TODO fixme
    return lhs.geom < rhs.geom;
  }

  friend std::ostream &operator<<(std::ostream &os, Geometry const &g) {
    os << g.toWKT();
    return os;
  }

private:
  void free();
};

#endif
