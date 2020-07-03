#ifndef MEOS_TYPES_BOX_STBOX_HPP
#define MEOS_TYPES_BOX_STBOX_HPP

#include <chrono>
#include <string>

using namespace std;

using time_point = std::chrono::system_clock::time_point;

class STBox {
public:
  STBox();
  STBox(string const &serialized);

  // XYZT
  STBox(double const xmin, double const ymin, double const zmin,
        time_point const tmin, double const xmax, double const ymax,
        double const zmax, time_point const tmax, bool const geodetic = false);
  STBox(double const xmin, double const ymin, double const zmin,
        string const &tmin, double const xmax, double const ymax,
        double const zmax, string const &tmax, bool const geodetic = false);

  // XYZ
  STBox(double const xmin, double const ymin, double const zmin,
        double const xmax, double const ymax, double const zmax,
        bool const geodetic = false);

  // XYT - geodetic not allowed
  STBox(double const xmin, double const ymin, time_point const tmin,
        double const xmax, double const ymax, time_point const tmax);
  STBox(double const xmin, double const ymin, string const &tmin,
        double const xmax, double const ymax, string const &tmax);

  // XY - geodetic not allowed
  STBox(double const xmin, double const ymin, double const xmax,
        double const ymax);

  // T
  STBox(time_point const tmin, time_point const tmax,
        bool const geodetic = false);

  double xmin() const;
  double ymin() const;
  double zmin() const;
  time_point tmin() const;
  double xmax() const;
  double ymax() const;
  double zmax() const;
  time_point tmax() const;

  bool geodetic() const;

  friend bool operator==(STBox const &lhs, STBox const &rhs);
  friend bool operator!=(STBox const &lhs, STBox const &rhs);
  friend bool operator<(STBox const &lhs, STBox const &rhs);
  friend bool operator>(STBox const &lhs, STBox const &rhs);
  friend bool operator>=(STBox const &lhs, STBox const &rhs);
  friend bool operator<=(STBox const &lhs, STBox const &rhs);

  friend istream &operator>>(istream &in, STBox &period);
  friend ostream &operator<<(ostream &os, STBox const &period);

private:
  double m_xmin = -INFINITY;
  double m_ymin = -INFINITY;
  double m_zmin = -INFINITY;
  time_point m_tmin = time_point(time_point::duration::min());
  double m_xmax = INFINITY;
  double m_ymax = INFINITY;
  double m_zmax = INFINITY;
  time_point m_tmax = time_point(time_point::duration::max());

  bool m_geodetic = false;

  /**
   * @return true if X bounds are finite
   */
  bool xset() const;

  /**
   * @return true if Y bounds are finite
   */
  bool yset() const;

  /**
   * @return true if Z bounds are finite
   */
  bool zset() const;

  /**
   * @return true if T bounds are finite
   */
  bool tset() const;

  void validate() const;
  int compare(STBox const &other) const;
};

#endif
