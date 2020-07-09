#ifndef MEOS_TYPES_GEOM_GEOMETRY_HPP
#define MEOS_TYPES_GEOM_GEOMETRY_HPP

#include <iosfwd>
#include <meos/geos.h>
#include <string>

/**
 * Mostly just a wrapper on top of GEOSGeometry*
 * Main reason we have this is that pybind11 can't work with GEOSGeometry*
 * directly, as it is exposed as an opaque type from the GEOS C API
 *
 * We only use this class to represent Points for now
 * Probably we should either generalize for any geometry or rename this to Point
 */
class Geometry {
public:
  GEOSGeometry *geom = nullptr;

  Geometry();
  Geometry(std::string wkt);
  Geometry(double x, double y);
  Geometry(Geometry const &g);
  void operator=(Geometry const &g);
  ~Geometry();

  void fromWKT(std::string wkt);
  std::string toWKT() const;

  double x() const;
  double y() const;

  Geometry operator+(Geometry const &g) const;
  Geometry operator-(Geometry const &g) const;

  int compare(Geometry const &other) const;

  friend bool operator==(Geometry const &lhs, Geometry const &rhs);
  friend bool operator!=(Geometry const &lhs, Geometry const &rhs);
  friend bool operator<(Geometry const &lhs, Geometry const &rhs);
  friend bool operator<=(Geometry const &lhs, Geometry const &rhs);
  friend bool operator>(Geometry const &lhs, Geometry const &rhs);
  friend bool operator>=(Geometry const &lhs, Geometry const &rhs);

  friend std::ostream &operator<<(std::ostream &os, Geometry const &g);

private:
  void point(double x, double y);
  void free();
};

#endif
