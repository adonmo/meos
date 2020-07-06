#include <cmath>
#include <meos/io/utils.hpp>
#include <meos/types/box/STBox.hpp>
#include <meos/util/serializing.hpp>
#include <sstream>

extern int const SRID_DEFAULT = 0;

STBox::STBox() {}

// XYZT
STBox::STBox(double const xmin, double const ymin, double const zmin,
             time_point const tmin, double const xmax, double const ymax,
             double const zmax, time_point const tmax, int const srid,
             bool const geodetic)
    : m_xmin(xmin), m_ymin(ymin), m_zmin(zmin), m_tmin(tmin), m_xmax(xmax),
      m_ymax(ymax), m_zmax(zmax), m_tmax(tmax), m_srid(srid),
      m_geodetic(geodetic) {
  init();
}

// XYZT
STBox::STBox(double const xmin, double const ymin, double const zmin,
             string const &tmin, double const xmax, double const ymax,
             double const zmax, string const &tmax, int const srid,
             bool const geodetic)
    : m_xmin(xmin), m_ymin(ymin), m_zmin(zmin), m_xmax(xmax), m_ymax(ymax),
      m_zmax(zmax), m_srid(srid), m_geodetic(geodetic) {
  stringstream tmin_ss(tmin);
  this->m_tmin = nextTime(tmin_ss);
  stringstream tmax_ss(tmax);
  this->m_tmax = nextTime(tmax_ss);
  init();
}

// XYZ
STBox::STBox(double const xmin, double const ymin, double const zmin,
             double const xmax, double const ymax, double const zmax,
             int const srid, bool const geodetic)
    : m_xmin(xmin), m_ymin(ymin), m_zmin(zmin), m_xmax(xmax), m_ymax(ymax),
      m_zmax(zmax), m_srid(srid), m_geodetic(geodetic) {
  init();
}

// XYT
STBox::STBox(double const xmin, double const ymin, time_point const tmin,
             double const xmax, double const ymax, time_point const tmax,
             int const srid)
    : m_xmin(xmin), m_ymin(ymin), m_tmin(tmin), m_xmax(xmax), m_ymax(ymax),
      m_tmax(tmax), m_srid(srid) {
  init();
}

// XYT
STBox::STBox(double const xmin, double const ymin, string const &tmin,
             double const xmax, double const ymax, string const &tmax,
             int const srid)
    : m_xmin(xmin), m_ymin(ymin), m_xmax(xmax), m_ymax(ymax), m_srid(srid) {
  stringstream tmin_ss(tmin);
  this->m_tmin = nextTime(tmin_ss);
  stringstream tmax_ss(tmax);
  this->m_tmax = nextTime(tmax_ss);
  init();
}

// XY
STBox::STBox(double const xmin, double const ymin, double const xmax,
             double const ymax, int const srid)
    : m_xmin(xmin), m_ymin(ymin), m_xmax(xmax), m_ymax(ymax), m_srid(srid) {
  init();
}

// T
STBox::STBox(time_point const tmin, time_point const tmax, int const srid,
             bool const geodetic)
    : m_tmin(tmin), m_tmax(tmax), m_srid(srid), m_geodetic(geodetic) {
  init();
}

STBox::STBox(string const &tmin, string const &tmax, int const srid,
             bool const geodetic)
    : m_srid(srid), m_geodetic(geodetic) {
  stringstream tmin_ss(tmin);
  this->m_tmin = nextTime(tmin_ss);
  stringstream tmax_ss(tmax);
  this->m_tmax = nextTime(tmax_ss);
  init();
}

STBox::STBox(string const &serialized) {
  stringstream ss(serialized);
  STBox stbox;
  ss >> stbox;
  this->m_xmin = stbox.xmin();
  this->m_ymin = stbox.ymin();
  this->m_zmin = stbox.zmin();
  this->m_tmin = stbox.tmin();
  this->m_xmax = stbox.xmax();
  this->m_ymax = stbox.ymax();
  this->m_zmax = stbox.zmax();
  this->m_tmax = stbox.tmax();
  this->m_srid = stbox.srid();
  this->m_geodetic = stbox.geodetic();
  init();
}

void STBox::init() {
  setup_defaults();
  validate();
}

void STBox::setup_defaults() {
  if (this->m_geodetic && this->has_x() && this->m_srid == SRID_DEFAULT) {
    this->m_srid = 4326;
  }
}

void STBox::validate() const {
  if (!this->has_x() && this->has_srid()) {
    throw invalid_argument("SRID is specified but coordinates are not given");
  }

  if (this->m_xmin > this->m_xmax) {
    throw invalid_argument("The xmin must be less than or equal to the xmax");
  }

  if (this->m_ymin > this->m_ymax) {
    throw invalid_argument("The ymin must be less than or equal to the ymax");
  }

  if (this->m_zmin > this->m_zmax) {
    throw invalid_argument("The zmin must be less than or equal to the zmax");
  }

  if (this->m_tmin > this->m_tmax) {
    throw invalid_argument("The tmin must be less than or equal to the tmax");
  }
}

double STBox::xmin() const { return this->m_xmin; }
double STBox::ymin() const { return this->m_ymin; }
double STBox::zmin() const { return this->m_zmin; }
time_point STBox::tmin() const { return this->m_tmin; }
double STBox::xmax() const { return this->m_xmax; }
double STBox::ymax() const { return this->m_ymax; }
double STBox::zmax() const { return this->m_zmax; }
time_point STBox::tmax() const { return this->m_tmax; }

int STBox::srid() const { return this->m_srid; }
bool STBox::geodetic() const { return this->m_geodetic; }

bool STBox::has_x() const { return this->m_xmin != -INFINITY; }
bool STBox::has_y() const { return this->m_ymin != -INFINITY; }
bool STBox::has_z() const { return this->m_zmin != -INFINITY; }
bool STBox::has_t() const {
  return this->m_tmin != time_point(time_point::duration::min());
}
bool STBox::has_srid() const { return this->m_srid != SRID_DEFAULT; }

int STBox::compare(STBox const &other) const {
  if (srid() < other.srid())
    return -1;
  else if (srid() > other.srid())
    return 1;
  else if (tmin() < other.tmin())
    return -1;
  else if (tmin() > other.tmin())
    return 1;
  else if (xmin() < other.xmin())
    return -1;
  else if (xmin() > other.xmin())
    return 1;
  else if (ymin() < other.ymin())
    return -1;
  else if (ymin() > other.ymin())
    return 1;
  else if (zmin() < other.zmin())
    return -1;
  else if (zmin() > other.zmin())
    return 1;
  else if (tmax() < other.tmax())
    return -1;
  else if (tmax() > other.tmax())
    return 1;
  else if (xmax() < other.xmax())
    return -1;
  else if (xmax() > other.xmax())
    return 1;
  else if (ymax() < other.ymax())
    return -1;
  else if (ymax() > other.ymax())
    return 1;
  else if (zmax() < other.zmax())
    return -1;
  else if (zmax() > other.zmax())
    return 1;
  else if (geodetic() < other.geodetic())
    return -1;
  else if (geodetic() > other.geodetic())
    return 1;
  return 0;
}

bool operator==(STBox const &lhs, STBox const &rhs) {
  return lhs.compare(rhs) == 0;
}

bool operator!=(STBox const &lhs, STBox const &rhs) {
  return lhs.compare(rhs) != 0;
}

bool operator<(STBox const &lhs, STBox const &rhs) {
  return lhs.compare(rhs) == -1;
}

bool operator>(STBox const &lhs, STBox const &rhs) { return rhs < lhs; }

bool operator>=(STBox const &lhs, STBox const &rhs) { return !(lhs < rhs); }

bool operator<=(STBox const &lhs, STBox const &rhs) { return !(rhs < lhs); }

istream &operator>>(istream &in, STBox &stbox) {
  double xmin = -INFINITY;
  double ymin = -INFINITY;
  double zmin = -INFINITY;
  time_point tmin = time_point(time_point::duration::min());

  double xmax = INFINITY;
  double ymax = INFINITY;
  double zmax = INFINITY;
  time_point tmax = time_point(time_point::duration::max());

  int srid = 0;
  bool geod = false;
  bool hasz = false;
  bool hast = false;

  in >> std::ws;

  int pos = in.tellg();
  char prefix[4];
  in.read(prefix, 4);
  bool srid_specified = string(prefix) == "SRID";
  if (srid_specified) {
    consume(in, '=');
    in >> srid;
    consume(in, ';');
  } else {
    in.seekg(pos);
  }

  if (in.peek() == 'G') {
    geod = true;
    hasz = true;
    consume(in, "GEODSTBOX");
    in >> std::ws;
    hast = in.peek() == 'T';
    if (hast)
      consume(in, 'T');
  } else {
    consume(in, "STBOX");
    in >> std::ws;
    hasz = in.peek() == 'Z';
    if (hasz)
      consume(in, 'Z');
    hast = in.peek() == 'T';
    if (hast)
      consume(in, 'T');
  }

  consume(in, '(');

  // Special case: empty STBox - STBOX()
  // Note: Not in official MobilityDB spec
  in >> std::ws;
  if (in.peek() == ')') {
    consume(in, ')');
    return in;
  }

  consume(in, '(');

  // Read X and Y min values
  in >> std::ws;
  // if xmin is skipped, ymin, xmax and ymax would be too
  bool xskip = in.peek() == ',';
  if (xskip && !hast) {
    throw invalid_argument("both xmin and tmin cannot be missing");
  }
  if (!xskip) {
    in >> xmin;
  }
  consume(in, ',');
  if (!xskip) {
    in >> ymin;
  }

  // Read Z min value
  in >> std::ws;
  if (!xskip && hasz) {
    consume(in, ',');
    in >> zmin;
  }

  // Read T min value
  in >> std::ws;
  if (hast) {
    consume(in, ',');
    tmin = nextTime(in);
  }

  consume(in, ')');
  consume(in, ',');
  consume(in, '(');
  if (!xskip) {
    in >> xmax;
  }
  consume(in, ',');
  if (!xskip) {
    in >> ymax;
  }
  if (!xskip && hasz) {
    consume(in, ',');
    in >> zmax;
  }
  if (hast) {
    consume(in, ',');
    tmax = nextTime(in);
  }
  consume(in, ')');
  consume(in, ')');

  stbox.m_xmin = xmin;
  stbox.m_ymin = ymin;
  stbox.m_zmin = zmin;
  stbox.m_tmin = tmin;
  stbox.m_xmax = xmax;
  stbox.m_ymax = ymax;
  stbox.m_zmax = zmax;
  stbox.m_tmax = tmax;

  stbox.m_srid = srid;
  stbox.m_geodetic = geod;

  return in;
}

ostream &operator<<(ostream &os, STBox const &stbox) {
  if (stbox.has_x() && stbox.srid() != 0) {
    os << "SRID=" << stbox.srid() << ";";
  }

  if (stbox.geodetic()) {
    if (stbox.has_t()) {
      if (stbox.has_x()) {
        // GEODSTBOX T((11, 12, 13, 2000-01-01),
        //             (21, 22, 23, 2000-01-02))
        os << "GEODSTBOX T((" << stbox.xmin() << ", " << stbox.ymin() << ", "
           << stbox.zmin() << ", " << write_ISO8601_time(stbox.tmin()) << "), ("
           << stbox.xmax() << ", " << stbox.ymax() << ", " << stbox.zmax()
           << ", " << write_ISO8601_time(stbox.tmax()) << "))";
        return os;
      }
      // GEODSTBOX T((, 2000-01-01), (, 2000-01-02))
      os << "GEODSTBOX T(( , , " << write_ISO8601_time(stbox.tmin())
         << "), ( , , " << write_ISO8601_time(stbox.tmax()) << "))";
      return os;
    }
    // GEODSTBOX((11, 12, 13), (21, 22, 23))
    os << "GEODSTBOX((" << stbox.xmin() << ", " << stbox.ymin() << ", "
       << stbox.zmin() << "), (" << stbox.xmax() << ", " << stbox.ymax() << ", "
       << stbox.zmax() << "))";
    return os;
  } else {
    if (stbox.has_x() && stbox.has_z() && stbox.has_t()) {
      // STBOX ZT((11, 12, 13, 2000-01-01),
      //          (21, 22, 23, 2000-01-02))
      os << "STBOX ZT((" << stbox.xmin() << ", " << stbox.ymin() << ", "
         << stbox.zmin() << ", " << write_ISO8601_time(stbox.tmin()) << "), ("
         << stbox.xmax() << ", " << stbox.ymax() << ", " << stbox.zmax() << ", "
         << write_ISO8601_time(stbox.tmax()) << "))";
      return os;
    } else if (stbox.has_x() && stbox.has_z() && !stbox.has_t()) {
      // STBOX Z((11, 12, 13), (21, 22, 23))
      os << "STBOX Z((" << stbox.xmin() << ", " << stbox.ymin() << ", "
         << stbox.zmin() << "), (" << stbox.xmax() << ", " << stbox.ymax()
         << ", " << stbox.zmax() << "))";
      return os;
    } else if (stbox.has_x() && !stbox.has_z() && stbox.has_t()) {
      // STBOX T((11, 12, 2000-01-01), (21, 22, 2000-01-02))
      os << "STBOX T((" << stbox.xmin() << ", " << stbox.ymin() << ", "
         << write_ISO8601_time(stbox.tmin()) << "), (" << stbox.xmax() << ", "
         << stbox.ymax() << ", " << write_ISO8601_time(stbox.tmax()) << "))";
      return os;
    } else if (stbox.has_x() && !stbox.has_z() && !stbox.has_t()) {
      // STBOX((11, 12), (21, 22))
      os << "STBOX((" << stbox.xmin() << ", " << stbox.ymin() << "), ("
         << stbox.xmax() << ", " << stbox.ymax() << "))";
      return os;
    } else if (!stbox.has_x() && !stbox.has_z() && stbox.has_t()) {
      // STBOX T((, 2000-01-01), (, 2000-01-02))
      os << "STBOX T(( , , " << write_ISO8601_time(stbox.tmin()) << "), ( , , "
         << write_ISO8601_time(stbox.tmax()) << "))";
      return os;
    } else if (!stbox.has_x() && !stbox.has_z() && !stbox.has_t()) {
      // Note: This is not part of official MobilityDB spec
      // Empty STBOX
      os << "STBOX()";
      return os;
    }
  }
  throw invalid_argument("ERROR: Wrong values");
}
