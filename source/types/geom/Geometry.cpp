#include <cstdlib>
#include <meos/types/geom/Geometry.hpp>

Geometry::Geometry() { point(0, 0); }
Geometry::Geometry(std::string wkt) { fromWKT(wkt); }
Geometry::Geometry(double x, double y) { point(x, y); }
Geometry::Geometry(Geometry const &g) {
  free();
  geom = GEOSGeom_clone_r(geos_context, g.geom);
}
void Geometry::operator=(Geometry const &g) {
  free();
  geom = GEOSGeom_clone_r(geos_context, g.geom);
}
Geometry::~Geometry() { free(); }

void Geometry::point(double x, double y) {
  free();
  auto seq = GEOSCoordSeq_create_r(geos_context, 1, 2);
  GEOSCoordSeq_setX_r(geos_context, seq, 0, x);
  GEOSCoordSeq_setY_r(geos_context, seq, 0, y);
  geom = GEOSGeom_createPoint_r(geos_context, seq);
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

double Geometry::x() const {
  double x;
  GEOSGeomGetX_r(geos_context, this->geom, &x);
  return x;
}

double Geometry::y() const {
  double y;
  GEOSGeomGetY_r(geos_context, this->geom, &y);
  return y;
}

Geometry Geometry::operator+(Geometry const &other) const {
  double x, y;
  GEOSGeomGetX_r(geos_context, this->geom, &x);
  GEOSGeomGetY_r(geos_context, this->geom, &y);

  double ox, oy;
  GEOSGeomGetX_r(geos_context, other.geom, &ox);
  GEOSGeomGetY_r(geos_context, other.geom, &oy);

  return Geometry(x + ox, y + oy);
}

Geometry Geometry::operator-(Geometry const &other) const {
  double x, y;
  GEOSGeomGetX_r(geos_context, this->geom, &x);
  GEOSGeomGetY_r(geos_context, this->geom, &y);

  double ox, oy;
  GEOSGeomGetX_r(geos_context, other.geom, &ox);
  GEOSGeomGetY_r(geos_context, other.geom, &oy);

  return Geometry(x - ox, y - oy);
}

int Geometry::compare(Geometry const &other) const {
  double x, y;
  GEOSGeomGetX_r(geos_context, this->geom, &x);
  GEOSGeomGetY_r(geos_context, this->geom, &y);

  double ox, oy;
  GEOSGeomGetX_r(geos_context, other.geom, &ox);
  GEOSGeomGetY_r(geos_context, other.geom, &oy);

  if (x < ox)
    return -1;
  else if (x > ox)
    return 1;
  else if (y < oy)
    return -1;
  else if (y > oy)
    return 1;

  return 0;
}

bool operator==(Geometry const &lhs, Geometry const &rhs) {
  return lhs.compare(rhs) == 0;
}

bool operator!=(Geometry const &lhs, Geometry const &rhs) {
  return lhs.compare(rhs) != 0;
}

bool operator<(Geometry const &lhs, Geometry const &rhs) {
  return lhs.compare(rhs) == -1;
}

bool operator>(Geometry const &lhs, Geometry const &rhs) { return rhs < lhs; }

bool operator>=(Geometry const &lhs, Geometry const &rhs) {
  return !(lhs < rhs);
}

bool operator<=(Geometry const &lhs, Geometry const &rhs) {
  return !(rhs < lhs);
}

std::ostream &operator<<(std::ostream &os, Geometry const &g) {
  os << g.toWKT();
  return os;
}
