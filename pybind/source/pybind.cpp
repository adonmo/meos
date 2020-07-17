#include <sstream>
#include <string>

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <meos/geos.hpp>

#include "box_module.hpp"
#include "common.hpp"
#include "io_module.hpp"
#include "range_module.hpp"
#include "temporal_module.hpp"
#include "time_module.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pymeos, m) {
  // TODO will we ever get a chance to do finish_geos()?
  init_geos();

  py::class_<Geometry>(m, "Geometry")
      .def(py::init<std::string>())
      .def(py::init<double, double>())
      .def(py::init<std::string, int>())
      .def(py::init<double, double, int>())
      .def(py::init<Geometry>())
      .def(py::self + py::self, py::arg("other"))
      .def(py::self - py::self, py::arg("other"))
      .def(py::self == py::self, py::arg("other"))
      .def(py::self != py::self, py::arg("other"))
      .def(py::self < py::self, py::arg("other"))
      .def(py::self <= py::self, py::arg("other"))
      .def(py::self > py::self, py::arg("other"))
      .def(py::self >= py::self, py::arg("other"))
      .def("__str__", &to_ostream<Geometry>)
      .def("__repr__", &to_ostream<Geometry>)
      .def("compare", &Geometry::compare, py::arg("other"))
      .def_property_readonly("x", &Geometry::x)
      .def_property_readonly("y", &Geometry::y)
      .def("fromWKT", &Geometry::fromWKT)
      .def("toWKT", &Geometry::toWKT);

  def_box_module(m);
  def_io_module(m);
  def_range_module(m);
  def_temporal_module(m);
  def_time_module(m);
}
