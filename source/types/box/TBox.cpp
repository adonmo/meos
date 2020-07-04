#include <meos/io/utils.hpp>
#include <meos/types/box/TBox.hpp>
#include <meos/util/serializing.hpp>
#include <sstream>

TBox::TBox() {}

TBox::TBox(double const xmin, time_point const tmin, double const xmax,
           time_point const tmax)
    : m_xmin(xmin), m_tmin(tmin), m_xmax(xmax), m_tmax(tmax) {
  validate();
}

TBox::TBox(double const xmin, double const xmax) : m_xmin(xmin), m_xmax(xmax) {
  validate();
}

TBox::TBox(time_point const tmin, time_point const tmax)
    : m_tmin(tmin), m_tmax(tmax) {
  validate();
}

TBox::TBox(double const xmin, string const &tmin, double const xmax,
           string const &tmax)
    : m_xmin(xmin), m_xmax(xmax) {
  stringstream lss(tmin);
  this->m_tmin = nextTime(lss);
  stringstream uss(tmax);
  this->m_tmax = nextTime(uss);
  validate();
}

TBox::TBox(string const &xmin, string const &tmin, string const &xmax,
           string const &tmax)
    : m_xmin(atof(xmin.c_str())), m_xmax(atof(xmax.c_str())) {
  stringstream lss(tmin);
  this->m_tmin = nextTime(lss);
  stringstream uss(tmax);
  this->m_tmax = nextTime(uss);
  validate();
}

TBox::TBox(string const &serialized) {
  stringstream ss(serialized);
  TBox tbox;
  ss >> tbox;
  this->m_xmin = tbox.xmin();
  this->m_tmin = tbox.tmin();
  this->m_xmax = tbox.xmax();
  this->m_tmax = tbox.tmax();
  validate();
}

void TBox::validate() const {
  if (this->xmin() > this->xmax()) {
    throw invalid_argument("The xmin must be less than or equal to the xmax");
  }

  if (this->tmin() > this->tmax()) {
    throw invalid_argument("The tmin must be less than or equal to the tmax");
  }
}

double TBox::xmin() const { return this->m_xmin; }
time_point TBox::tmin() const { return this->m_tmin; }
double TBox::xmax() const { return this->m_xmax; }
time_point TBox::tmax() const { return this->m_tmax; }

bool TBox::xset() const { return this->m_xmin != -INFINITY; }
bool TBox::tset() const {
  return this->m_tmin != time_point(time_point::duration::min());
}

int TBox::compare(TBox const &other) const {
  if (tmin() < other.tmin())
    return -1;
  else if (tmin() > other.tmin())
    return 1;
  else if (xmin() < other.xmin())
    return -1;
  else if (xmin() > other.xmin())
    return 1;
  else if (tmax() < other.tmax())
    return -1;
  else if (tmax() > other.tmax())
    return 1;
  else if (xmax() < other.xmax())
    return -1;
  else if (xmax() > other.xmax())
    return 1;
  return 0;
}

bool operator==(TBox const &lhs, TBox const &rhs) {
  return lhs.compare(rhs) == 0;
}

bool operator!=(TBox const &lhs, TBox const &rhs) {
  return lhs.compare(rhs) != 0;
}

bool operator<(TBox const &lhs, TBox const &rhs) {
  return lhs.compare(rhs) == -1;
}

bool operator>(TBox const &lhs, TBox const &rhs) { return rhs < lhs; }

bool operator>=(TBox const &lhs, TBox const &rhs) { return !(lhs < rhs); }

bool operator<=(TBox const &lhs, TBox const &rhs) { return !(rhs < lhs); }

istream &operator>>(istream &in, TBox &tbox) {
  double xmin = -INFINITY;
  double xmax = INFINITY;
  time_point tmin = time_point(time_point::duration::min());
  time_point tmax = time_point(time_point::duration::max());

  in >> std::ws;
  consume(in, "TBOX");

  consume(in, '(');

  // Special case: empty STBox - STBOX()
  // Note: Not in official MobilityDB spec
  in >> std::ws;
  if (in.peek() == ')') {
    consume(in, ')');
    return in;
  }

  consume(in, '(');

  in >> std::ws;
  // if xmin is skipped, xmax would be too
  bool xskip = in.peek() == ',';
  if (!xskip) {
    in >> xmin;
  }
  consume(in, ',');

  in >> std::ws;
  // if tmin is skipped, tmax would be too
  bool tskip = in.peek() == ')';
  if (xskip && tskip) {
    throw invalid_argument("both xmin and tmin cannot be missing");
  }
  if (!tskip) {
    tmin = nextTime(in);
  }

  consume(in, ')');
  consume(in, ',');
  consume(in, '(');
  if (!xskip) {
    in >> xmax;
  }
  consume(in, ',');
  if (!tskip) {
    tmax = nextTime(in);
  }
  consume(in, ')');
  consume(in, ')');

  tbox.m_tmin = tmin;
  tbox.m_tmax = tmax;
  tbox.m_xmin = xmin;
  tbox.m_xmax = xmax;

  return in;
}

ostream &operator<<(ostream &os, TBox const &tbox) {
  os << "TBOX(";

  if (tbox.xset() && tbox.tset()) {

    // Example: TBOX((1.0, 2000-01-01), (2.0, 2000-01-02))
    os << "(" << tbox.xmin() << ", " << write_ISO8601_time(tbox.tmin())
       << "), (" << tbox.xmax() << ", " << write_ISO8601_time(tbox.tmax())
       << ")";

  } else if (tbox.xset()) {

    // Example: TBOX((1.0,), (2.0,))
    os << "(" << tbox.xmin() << ",), (" << tbox.xmax() << ",)";

  } else if (tbox.tset()) {

    // Example: TBOX((, 2000-01-01), (, 2000-01-02))
    os << "(, " << write_ISO8601_time(tbox.tmin()) << "), (, "
       << write_ISO8601_time(tbox.tmax()) << ")";
  }

  os << ")";
  return os;
}
