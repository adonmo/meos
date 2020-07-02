#ifndef MEOS_TYPES_BOX_TBOX_HPP
#define MEOS_TYPES_BOX_TBOX_HPP

#include <chrono>
#include <string>

using namespace std;

using time_point = std::chrono::system_clock::time_point;

class TBox {
public:
  TBox();
  TBox(double const xmin, time_point const tmin, double const xmax,
       time_point const tmax);
  TBox(double const xmin, double const xmax);
  TBox(time_point const tmin, time_point const tmax);
  TBox(double const xmin, string const &tmin, double const xmax,
       string const &tmax);
  TBox(string const &xmin, string const &tmin, string const &xmax,
       string const &tmax);
  TBox(string const &serialized);

  time_point tmin() const;
  time_point tmax() const;
  double xmin() const;
  double xmax() const;

  friend bool operator==(TBox const &lhs, TBox const &rhs);
  friend bool operator!=(TBox const &lhs, TBox const &rhs);
  friend bool operator<(TBox const &lhs, TBox const &rhs);
  friend bool operator>(TBox const &lhs, TBox const &rhs);
  friend bool operator>=(TBox const &lhs, TBox const &rhs);
  friend bool operator<=(TBox const &lhs, TBox const &rhs);

  friend istream &operator>>(istream &in, TBox &period);
  friend ostream &operator<<(ostream &os, TBox const &period);

private:
  double m_xmin;
  time_point m_tmin;
  double m_xmax;
  time_point m_tmax;

  void validate() const;
  int compare(TBox const &other) const;
};

#endif
