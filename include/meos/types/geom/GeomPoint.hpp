#pragma once

#include <meos/geos.hpp>
#include <string>

namespace meos {

/**
 * Mostly a wrapper on top of GEOSGeometry*
 * Additionally, we allow specifying SRID, similar to how PostGIS EWKT does.
 *
 * Main reason we have this is that pybind11 can't work with GEOSGeometry*
 * directly, as it is exposed as an opaque type from the GEOS C API.
 */
class GeomPoint {
public:
  GEOSGeometry *geom = nullptr;

  GeomPoint();
  GeomPoint(std::string serialized);
  GeomPoint(double x, double y);
  GeomPoint(std::string serialized, int srid);
  GeomPoint(double x, double y, int srid);
  GeomPoint(GeomPoint const &g);
  void operator=(GeomPoint const &g);
  ~GeomPoint();

  void fromWKB(std::istream &is);
  void toWKB(std::ostream &os) const;

  void fromWKT(std::string wkt);
  std::string toWKT() const;

  void fromHEX(std::istream &is);
  void toHEX(std::ostream &os) const;

  double x() const;
  double y() const;
  int srid() const;

  GeomPoint operator+(GeomPoint const &g) const;
  GeomPoint operator-(GeomPoint const &g) const;

  int compare(GeomPoint const &other) const;

  friend bool operator==(GeomPoint const &lhs, GeomPoint const &rhs);
  friend bool operator!=(GeomPoint const &lhs, GeomPoint const &rhs);
  friend bool operator<(GeomPoint const &lhs, GeomPoint const &rhs);
  friend bool operator<=(GeomPoint const &lhs, GeomPoint const &rhs);
  friend bool operator>(GeomPoint const &lhs, GeomPoint const &rhs);
  friend bool operator>=(GeomPoint const &lhs, GeomPoint const &rhs);

  friend std::istream &operator>>(std::istream &in, GeomPoint &g);
  friend std::ostream &operator<<(std::ostream &os, GeomPoint const &g);

private:
  void point(double x, double y);
  void free();
};

/**
 * Helps find out if a base type is geometry or not
 *
 * Also check is_geometry_v<BaseType>, which should be more easy to use
 */
template <typename BaseType> struct is_geometry { static const bool value = false; };
template <> struct is_geometry<GeomPoint> {
  static const bool value = true;
  typedef GeomPoint type;
};

/**
 * Shorthand for is_geometry<BaseType>::value
 */
template <typename BaseType> constexpr bool is_geometry_v = is_geometry<BaseType>::value;

}  // namespace meos
