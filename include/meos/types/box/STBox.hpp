#pragma once

#include <chrono>
#include <meos/types/geom/SRIDMembers.hpp>
#include <string>

namespace meos {
using time_point = std::chrono::system_clock::time_point;

class STBox : public SRIDMembers {
public:
  STBox();
  STBox(std::string const &serialized);

  // XYZT
  STBox(double const xmin, double const ymin, double const zmin, time_point const tmin,
        double const xmax, double const ymax, double const zmax, time_point const tmax,
        int const srid = 0, bool const geodetic = false);
  STBox(double const xmin, double const ymin, double const zmin, std::string const &tmin,
        double const xmax, double const ymax, double const zmax, std::string const &tmax,
        int const srid = 0, bool const geodetic = false);

  // XYZ
  STBox(double const xmin, double const ymin, double const zmin, double const xmax,
        double const ymax, double const zmax, int const srid = 0, bool const geodetic = false);

  // XYT - geodetic not allowed
  STBox(double const xmin, double const ymin, time_point const tmin, double const xmax,
        double const ymax, time_point const tmax, int const srid = 0);
  STBox(double const xmin, double const ymin, std::string const &tmin, double const xmax,
        double const ymax, std::string const &tmax, int const srid = 0);

  // XY - geodetic not allowed
  STBox(double const xmin, double const ymin, double const xmax, double const ymax,
        int const srid = 0);

  // T
  STBox(time_point const tmin, time_point const tmax, int const srid = 0,
        bool const geodetic = false);
  STBox(std::string const &tmin, std::string const &tmax, int const srid = 0,
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

  friend std::istream &operator>>(std::istream &in, STBox &period);
  friend std::ostream &operator<<(std::ostream &os, STBox const &period);

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
  bool has_x() const;

  /**
   * @return true if Y bounds are finite
   */
  bool has_y() const;

  /**
   * @return true if Z bounds are finite
   */
  bool has_z() const;

  /**
   * @return true if T bounds are finite
   */
  bool has_t() const;

  void init();
  void setup_defaults();
  void validate() const;
  int compare(STBox const &other) const;
};

}  // namespace meos
