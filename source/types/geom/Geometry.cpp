#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <meos/io/utils.hpp>
#include <meos/types/geom/Geometry.hpp>
#include <meos/util/string.hpp>
#include <sstream>
#include <string>

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
    throw std::invalid_argument("Conflicting SRIDs provided. Given: " + std::to_string(srid)
                                + ", while Geometry contains: " + std::to_string(g.srid()));
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

void Geometry::fromWKB(std::istream &is) {
  free();
  const size_t BUFFER_SIZE = 2048;
  unsigned char *buf = new unsigned char[BUFFER_SIZE];

  is.read(reinterpret_cast<char *>(buf), BUFFER_SIZE);
  auto length = is.gcount();
  buf[length] = '\0';
  geom = GEOSGeomFromWKB_buf_r(geos_context, buf, length);

  delete[] buf;
  if (geom == nullptr) {
    throw std::invalid_argument("Could not parse WKB (binary)");
  }
}

void Geometry::toWKB(std::ostream &os) const {
  if (geom != nullptr) {
    GEOSWKBWriter *wkbw_ = GEOSWKBWriter_create_r(geos_context);
    size_t size;
    unsigned char *wkb_c = GEOSWKBWriter_write_r(geos_context, wkbw_, geom, &size);
    os.write(reinterpret_cast<char *>(wkb_c), size);
    std::free(wkb_c);
    GEOSWKBWriter_destroy_r(geos_context, wkbw_);
  } else {
    throw "Geometry not initiated.";
  }
}

void Geometry::fromWKT(std::string wkt) {
  free();
  geom = GEOSGeomFromWKT_r(geos_context, wkt.c_str());
  if (geom == nullptr) {
    throw std::invalid_argument("Could not parse WKT");
  }
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
#include <iostream>
void Geometry::fromHEX(std::istream &is) {
  free();
  const size_t BUFFER_SIZE = 2048;
  unsigned char *buf = new unsigned char[BUFFER_SIZE];

  is.read(reinterpret_cast<char *>(buf), BUFFER_SIZE);
  auto length = is.gcount();
  buf[length] = '\0';
  geom = GEOSGeomFromHEX_buf_r(geos_context, buf, length);

  delete[] buf;
  if (geom == nullptr) {
    throw std::invalid_argument("Could not parse WKB (hex)");
  }
}

void Geometry::toHEX(std::ostream &os) const {
  if (geom != nullptr) {
    GEOSWKBWriter *wkbw_ = GEOSWKBWriter_create_r(geos_context);
    size_t size;
    unsigned char *wkb_c = GEOSWKBWriter_writeHEX_r(geos_context, wkbw_, geom, &size);
    os.write(reinterpret_cast<char *>(wkb_c), size);
    std::free(wkb_c);
    GEOSWKBWriter_destroy_r(geos_context, wkbw_);
  } else {
    throw "Geometry not initiated.";
  }
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

bool operator==(Geometry const &lhs, Geometry const &rhs) { return lhs.compare(rhs) == 0; }

bool operator!=(Geometry const &lhs, Geometry const &rhs) { return lhs.compare(rhs) != 0; }

bool operator<(Geometry const &lhs, Geometry const &rhs) { return lhs.compare(rhs) == -1; }

bool operator>(Geometry const &lhs, Geometry const &rhs) { return rhs < lhs; }

bool operator>=(Geometry const &lhs, Geometry const &rhs) { return !(lhs < rhs); }

bool operator<=(Geometry const &lhs, Geometry const &rhs) { return !(rhs < lhs); }

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

  // Geometries could be in either WKT or WKB (Hex) formats
  // So first we detect which one of them is the case
  in >> std::ws;
  auto start_pos = in.tellg();
  auto buffer = read_until_one_of(in, "(@");
  auto final_pos = in.tellg();
  auto length = final_pos - start_pos;
  bool is_wkt = in.peek() == '(';

  if (is_wkt) {
    // WKT follows the pattern GEOMTYPENAME(...), with possible nested braces
    // This piece of code reads until all braces are closed

    std::transform(buffer.begin(), buffer.end(), buffer.begin(), toupper);
    if (trim(buffer) != "POINT") {
      throw std::invalid_argument("Only POINT geometry supported as of now");
    }

    consume(in, '(');
    int brace_nesting = 1;
    while (brace_nesting != 0) {
      read_until_one_of(in, "()");
      brace_nesting += (in.get() == '(' ? 1 : -1);
    }
    final_pos = in.tellg();
    length = final_pos - start_pos;
    in.seekg(start_pos);

    char *buf = new char[length + 1];
    in.read(buf, length);
    buf[length] = '\0';

    g.geom = GEOSGeomFromWKT_r(geos_context, buf);

    delete[] buf;
  } else {
    std::stringstream ss(buffer);
    g.fromHEX(ss);
  }

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
