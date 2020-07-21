#ifndef MEOS_TYPES_GEOM_GEOMETRY_HPP
#define MEOS_TYPES_GEOM_GEOMETRY_HPP

#include <meos/geos.hpp>
#include <string>

/**
 * Mostly a wrapper on top of GEOSGeometry*
 * Additionally, we allow specifying SRID, similar to how PostGIS EWKT does.
 *
 * Main reason we have this is that pybind11 can't work with GEOSGeometry*
 * directly, as it is exposed as an opaque type from the GEOS C API.
 *
 * We only use this class to represent Points for now
 * Probably we should either generalize for any geometry or rename this to Point
 */
class Geometry {
public:
  GEOSGeometry *geom = nullptr;

  Geometry();
  Geometry(std::string serialized);
  Geometry(double x, double y);
  Geometry(std::string serialized, int srid);
  Geometry(double x, double y, int srid);
  Geometry(Geometry const &g);
  void operator=(Geometry const &g);
  ~Geometry();

  void fromWKB(std::istream &is);
  void toWKB(std::ostream &os) const;

  void fromWKT(std::string wkt);
  std::string toWKT() const;

  void fromHEX(std::istream &is);
  void toHEX(std::ostream &os) const;

  double x() const;
  double y() const;
  int srid() const;

  Geometry operator+(Geometry const &g) const;
  Geometry operator-(Geometry const &g) const;

  int compare(Geometry const &other) const;

  friend bool operator==(Geometry const &lhs, Geometry const &rhs);
  friend bool operator!=(Geometry const &lhs, Geometry const &rhs);
  friend bool operator<(Geometry const &lhs, Geometry const &rhs);
  friend bool operator<=(Geometry const &lhs, Geometry const &rhs);
  friend bool operator>(Geometry const &lhs, Geometry const &rhs);
  friend bool operator>=(Geometry const &lhs, Geometry const &rhs);

  friend std::istream &operator>>(std::istream &in, Geometry &g);
  friend std::ostream &operator<<(std::ostream &os, Geometry const &g);

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
template <> struct is_geometry<Geometry> {
  static const bool value = true;
  typedef Geometry type;
};

/**
 * Shorthand for is_geometry<BaseType>::value
 */
template <typename BaseType> constexpr bool is_geometry_v = is_geometry<BaseType>::value;

#endif
