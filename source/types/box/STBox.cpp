#include <cmath>
#include <meos/io/utils.hpp>
#include <meos/types/box/STBox.hpp>
#include <meos/util/serializing.hpp>
#include <sstream>

STBox::STBox() {}

// XYZT
STBox::STBox(double const xmin, double const ymin, double const zmin,
             time_point const tmin, double const xmax, double const ymax,
             double const zmax, time_point const tmax, bool const geodetic)
    : m_xmin(xmin), m_ymin(ymin), m_zmin(zmin), m_tmin(tmin), m_xmax(xmax),
      m_ymax(ymax), m_zmax(zmax), m_tmax(tmax), m_geodetic(geodetic) {
  validate();
}

// XYZT
STBox::STBox(double const xmin, double const ymin, double const zmin,
             string const &tmin, double const xmax, double const ymax,
             double const zmax, string const &tmax, bool const geodetic)
    : m_xmin(xmin), m_ymin(ymin), m_zmin(zmin), m_xmax(xmax), m_ymax(ymax),
      m_zmax(zmax), m_geodetic(geodetic) {
  stringstream tmin_ss(tmin);
  this->m_tmin = nextTime(tmin_ss);
  stringstream tmax_ss(tmax);
  this->m_tmax = nextTime(tmax_ss);
  validate();
}

// XYZ
STBox::STBox(double const xmin, double const ymin, double const zmin,
             double const xmax, double const ymax, double const zmax,
             bool const geodetic)
    : m_xmin(xmin), m_ymin(ymin), m_zmin(zmin), m_xmax(xmax), m_ymax(ymax),
      m_zmax(zmax), m_geodetic(geodetic) {
  validate();
}

// XYT
STBox::STBox(double const xmin, double const ymin, time_point const tmin,
             double const xmax, double const ymax, time_point const tmax)
    : m_xmin(xmin), m_ymin(ymin), m_tmin(tmin), m_xmax(xmax), m_ymax(ymax),
      m_tmax(tmax) {
  validate();
}

// XYT
STBox::STBox(double const xmin, double const ymin, string const &tmin,
             double const xmax, double const ymax, string const &tmax)
    : m_xmin(xmin), m_ymin(ymin), m_xmax(xmax), m_ymax(ymax) {
  stringstream tmin_ss(tmin);
  this->m_tmin = nextTime(tmin_ss);
  stringstream tmax_ss(tmax);
  this->m_tmax = nextTime(tmax_ss);
  validate();
}

// XY
STBox::STBox(double const xmin, double const ymin, double const xmax,
             double const ymax)
    : m_xmin(xmin), m_ymin(ymin),
      m_tmin(time_point(time_point::duration::min())), m_xmax(xmax),
      m_ymax(ymax), m_tmax(time_point(time_point::duration::max())) {
  validate();
}

// T
STBox::STBox(time_point const tmin, time_point const tmax, bool const geodetic)
    : m_tmin(tmin), m_tmax(tmax), m_geodetic(geodetic) {
  validate();
}

STBox::STBox(string const &tmin, string const &tmax, bool const geodetic)
    : m_geodetic(geodetic) {
  stringstream tmin_ss(tmin);
  this->m_tmin = nextTime(tmin_ss);
  stringstream tmax_ss(tmax);
  this->m_tmax = nextTime(tmax_ss);
  validate();
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
  validate();
}

void STBox::validate() const {
  if (this->xmin() > this->xmax()) {
    throw invalid_argument("The xmin must be less than or equal to the xmax");
  }

  if (this->ymin() > this->ymax()) {
    throw invalid_argument("The ymin must be less than or equal to the ymax");
  }

  if (this->zmin() > this->zmax()) {
    throw invalid_argument("The zmin must be less than or equal to the zmax");
  }

  if (this->tmin() > this->tmax()) {
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

bool STBox::geodetic() const { return this->m_geodetic; }

bool STBox::xset() const { return this->m_xmin != -INFINITY; }
bool STBox::yset() const { return this->m_ymin != -INFINITY; }
bool STBox::zset() const { return this->m_zmin != -INFINITY; }
bool STBox::tset() const {
  return this->m_tmin != time_point(time_point::duration::min());
}

int STBox::compare(STBox const &other) const {
  if (tmin() < other.tmin())
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

  bool geod = false;
  bool hasz = false;
  bool hast = false;

  in >> std::ws;
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
  consume(in, '(');

  // Read X and Y min values
  in >> std::ws;
  // if xmin is skipped, ymin, xmax and ymax would be too
  bool xskip = in.peek() == ',';
  if (!xskip) {
    in >> xmin;
  }
  consume(in, ',');
  if (!xskip) {
    in >> ymin;
  }

  // Read Z min value
  consume(in, ',');
  in >> std::ws;
  // if zmin is skipped, zmax would be too, and hasz should reflect this
  bool zskip = in.peek() == ')';
  if (hasz == zskip) {
    throw invalid_argument("invalid z value");
  }
  if (hasz) {
    in >> zmin;
  }

  // Read T min value
  consume(in, ',');
  in >> std::ws;
  // if tmin is skipped, tmax would be too, and hast should reflect this
  bool tskip = in.peek() == ')';
  if (hast == tskip) {
    throw invalid_argument("invalid t value");
  }
  if (xskip && tskip) {
    throw invalid_argument("both xmin and tmin cannot be missing");
  }
  if (hast) {
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
  consume(in, ',');
  if (!zskip) {
    in >> zmax;
  }
  consume(in, ',');
  if (!tskip) {
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

  stbox.m_geodetic = geod;

  return in;
}

ostream &operator<<(ostream &os, STBox const &stbox) {
  if (stbox.geodetic()) {
    if (stbox.tset()) {
      if (stbox.xset()) {
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
    if (stbox.xset() && stbox.zset() && stbox.tset()) {
      // STBOX ZT((11, 12, 13, 2000-01-01),
      //          (21, 22, 23, 2000-01-02))
      os << "STBOX ZT((" << stbox.xmin() << ", " << stbox.ymin() << ", "
         << stbox.zmin() << ", " << write_ISO8601_time(stbox.tmin()) << "), ("
         << stbox.xmax() << ", " << stbox.ymax() << ", " << stbox.zmax() << ", "
         << write_ISO8601_time(stbox.tmax()) << "))";
      return os;
    } else if (stbox.xset() && stbox.zset() && !stbox.tset()) {
      // STBOX Z((11, 12, 13), (21, 22, 23))
      os << "STBOX Z((" << stbox.xmin() << ", " << stbox.ymin() << ", "
         << stbox.zmin() << "), (" << stbox.xmax() << ", " << stbox.ymax()
         << ", " << stbox.zmax() << "))";
      return os;
    } else if (stbox.xset() && !stbox.zset() && stbox.tset()) {
      // STBOX T((11, 12, 2000-01-01), (21, 22, 2000-01-02))
      os << "STBOX T((" << stbox.xmin() << ", " << stbox.ymin() << ", "
         << write_ISO8601_time(stbox.tmin()) << "), (" << stbox.xmax() << ", "
         << stbox.ymax() << ", " << write_ISO8601_time(stbox.tmax()) << "))";
      return os;
    } else if (stbox.xset() && !stbox.zset() && !stbox.tset()) {
      // STBOX((11, 12), (21, 22))
      os << "STBOX((" << stbox.xmin() << ", " << stbox.ymin() << "), ("
         << stbox.xmax() << ", " << stbox.ymax() << "))";
      return os;
    } else if (!stbox.xset() && !stbox.zset() && stbox.tset()) {
      // STBOX T((, 2000-01-01), (, 2000-01-02))
      os << "STBOX T(( , , " << write_ISO8601_time(stbox.tmin()) << "), ( , , "
         << write_ISO8601_time(stbox.tmax()) << "))";
      return os;
    } else if (!stbox.xset() && !stbox.zset() && !stbox.tset()) {
      // Note: This is not part of official MobilityDB spec
      // Empty STBOX
      os << "STBOX()";
      return os;
    }
  }
  throw invalid_argument("ERROR: Wrong values");
}
