#pragma once

#include <chrono>
#include <cmath>
#include <string>

namespace meos {
using time_point = std::chrono::system_clock::time_point;

class TBox {
public:
  TBox();
  TBox(double const xmin, time_point const tmin, double const xmax, time_point const tmax);
  TBox(double const xmin, double const xmax);
  TBox(time_point const tmin, time_point const tmax);
  TBox(double const xmin, std::string const &tmin, double const xmax, std::string const &tmax);
  TBox(std::string const &xmin, std::string const &tmin, std::string const &xmax,
       std::string const &tmax);
  TBox(std::string const &serialized);

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

  friend std::istream &operator>>(std::istream &in, TBox &period);
  friend std::ostream &operator<<(std::ostream &os, TBox const &period);

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

}  // namespace meos
