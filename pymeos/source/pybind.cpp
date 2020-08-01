#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <meos/geos.hpp>
#include <sstream>
#include <string>

#include "box_module.hpp"
#include "common.hpp"
#include "io_module.hpp"
#include "range_module.hpp"
#include "temporal_module.hpp"
#include "time_module.hpp"

namespace py = pybind11;

PYBIND11_MODULE(_pymeos, m) {
  // TODO will we ever get a chance to do finish_geos()?
  init_geos();

  py::class_<GeomPoint>(m, "GeomPoint")
      .def(py::init<std::string>())
      .def(py::init<double, double>())
      .def(py::init<std::string, int>())
      .def(py::init<double, double, int>())
      .def(py::init<GeomPoint>())
      .def(py::self + py::self, py::arg("other"))
      .def(py::self - py::self, py::arg("other"))
      .def(py::self == py::self, py::arg("other"))
      .def(py::self != py::self, py::arg("other"))
      .def(py::self < py::self, py::arg("other"))
      .def(py::self <= py::self, py::arg("other"))
      .def(py::self > py::self, py::arg("other"))
      .def(py::self >= py::self, py::arg("other"))
      .def("__str__", &to_ostream<GeomPoint>)
      .def("__repr__", &to_ostream<GeomPoint>)
      .def("compare", &GeomPoint::compare, py::arg("other"))
      .def_property_readonly("x", &GeomPoint::x)
      .def_property_readonly("y", &GeomPoint::y)
      .def_property_readonly("srid", &GeomPoint::srid)
      .def("fromWKB", &GeomPoint::fromWKB)
      .def("toWKB", &GeomPoint::toWKB)
      .def("fromWKT", &GeomPoint::fromWKT)
      .def("toWKT", &GeomPoint::toWKT)
      .def("fromHEX", &GeomPoint::fromHEX)
      .def("toHEX", &GeomPoint::toHEX);

  def_box_module(m);
  def_io_module(m);
  def_range_module(m);
  def_temporal_module(m);
  def_time_module(m);
}
