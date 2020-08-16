#pragma once

#include <pybind11/pybind11.h>

#include <meos/types/geom/GeomPoint.hpp>

namespace py = pybind11;

namespace pybind11 {
namespace detail {
// Casting between GeomPoint and shapely.geometry.point.Point
template <> class type_caster<GeomPoint> {
public:
  typedef GeomPoint type;
  bool load(handle src, bool) {
    if (!src) return false;
    GeomPoint point(src.attr("x").cast<double>(), src.attr("y").cast<double>());
    value = point;
    return true;
  }

  static handle cast(const GeomPoint &src, return_value_policy policy, handle /* parent */) {
    auto Point = py::module::import("shapely.geometry").attr("Point");
    py::object p = Point(src.x(), src.y());
    return p.inc_ref().ptr();
  }
  PYBIND11_TYPE_CASTER(type, _("shapely.geometry.point.Point"));
};
}  // namespace detail
}  // namespace pybind11
