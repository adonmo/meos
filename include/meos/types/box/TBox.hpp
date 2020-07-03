#ifndef MEOS_TYPES_BOX_TBOX_HPP
#define MEOS_TYPES_BOX_TBOX_HPP

#include <chrono>
#include <cmath>
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

  double xmin() const;
  time_point tmin() const;
  double xmax() const;
  time_point tmax() const;

  friend bool operator==(TBox const &lhs, TBox const &rhs);
  friend bool operator!=(TBox const &lhs, TBox const &rhs);
  friend bool operator<(TBox const &lhs, TBox const &rhs);
  friend bool operator>(TBox const &lhs, TBox const &rhs);
  friend bool operator>=(TBox const &lhs, TBox const &rhs);
  friend bool operator<=(TBox const &lhs, TBox const &rhs);

  friend istream &operator>>(istream &in, TBox &period);
  friend ostream &operator<<(ostream &os, TBox const &period);

private:
  double m_xmin = -INFINITY;
  time_point m_tmin = time_point(time_point::duration::min());
  double m_xmax = INFINITY;
  time_point m_tmax = time_point(time_point::duration::max());

  /**
   * @return true if X bounds are finite
   */
  bool xset() const;

  /**
   * @return true if T bounds are finite
   */
  bool tset() const;

  void validate() const;
  int compare(TBox const &other) const;
};

#endif
