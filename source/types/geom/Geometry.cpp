#include <cstdlib>
#include <iomanip>
#include <string>

#include <meos/io/utils.hpp>
#include <meos/types/geom/Geometry.hpp>

Geometry::Geometry() { point(0, 0); }

Geometry::Geometry(std::string serialized) {
  std::stringstream ss(serialized);
  Geometry g;
  ss >> g;
  this->geom = GEOSGeom_clone_r(geos_context, g.geom);
}

Geometry::Geometry(double x, double y) { point(x, y); }

Geometry::Geometry(std::string serialized, int srid) {
  std::stringstream ss(serialized);
  Geometry g;
  ss >> g;

  // If two different, non-zero SRIDs are provided, we throw an error
  if (g.srid() != srid && g.srid() * srid != 0) {
    throw std::invalid_argument(
        "Conflicting SRIDs provided. Given: " + std::to_string(srid) +
        ", while Geometry contains: " + std::to_string(g.srid()));
  }

  if (srid != 0) {
    GEOSSetSRID_r(geos_context, g.geom, srid);
  }

  this->geom = GEOSGeom_clone_r(geos_context, g.geom);
}

Geometry::Geometry(double x, double y, int srid) {
  point(x, y);
  GEOSSetSRID_r(geos_context, this->geom, srid);
}

Geometry::Geometry(Geometry const &g) {
  free();
  this->geom = GEOSGeom_clone_r(geos_context, g.geom);
}

void Geometry::operator=(Geometry const &g) {
  free();
  this->geom = GEOSGeom_clone_r(geos_context, g.geom);
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

int Geometry::srid() const { return GEOSGetSRID_r(geos_context, this->geom); }

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
  else if (srid() < other.srid())
    return -1;
  else if (srid() > other.srid())
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

std::istream &operator>>(std::istream &in, Geometry &g) {
  int srid = 0;

  // We first check if a SRID prefix is present
  in >> std::ws;
  int pos = in.tellg();
  char prefix[4];
  in.read(prefix, 4);
  bool srid_specified = std::string(prefix, 4) == "SRID";
  if (srid_specified) {
    consume(in, '=');
    in >> srid;
    consume(in, ';');
  } else {
    in.seekg(pos);
  }

  // Geometries are in the patter GEOMTYPENAME(...), with possible nested braces
  // This piece of code reads until all braces are closed
  in >> std::ws;
  size_t start_pos = in.tellg();
  read_until_one_of(in, "(");
  consume(in, '(');
  int brace_nesting = 1;
  while (brace_nesting != 0) {
    read_until_one_of(in, "()");
    brace_nesting += (in.get() == '(' ? 1 : -1);
  }
  size_t final_pos = in.tellg();
  size_t length = final_pos - start_pos;
  in.seekg(start_pos);

  char *wkt_buffer = new char[length + 1];
  in.read(wkt_buffer, length);
  wkt_buffer[length] = '\0';

  g.geom = GEOSGeomFromWKT_r(geos_context, wkt_buffer);
  delete[] wkt_buffer;
  GEOSSetSRID_r(geos_context, g.geom, srid);

  return in;
}

std::ostream &operator<<(std::ostream &os, Geometry const &g) {
  if (g.srid() != 0) {
    os << "SRID=" << g.srid() << ";";
  }

  os << g.toWKT();
  return os;
}
