#pragma once

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <meos/types/geom/GeomPoint.hpp>
#include <string>

#include "common.hpp"

using namespace meos;
namespace py = pybind11;

void def_geompoint_class(py::module &m) {
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
      .def("fromEWKB", &GeomPoint::fromEWKB)
      .def("toEWKB", &GeomPoint::toEWKB)
      .def("fromEWKT", &GeomPoint::fromEWKT)
      .def("toEWKT", &GeomPoint::toEWKT)
      .def("fromWKB", &GeomPoint::fromWKB)
      .def("toWKB", &GeomPoint::toWKB, py::arg("ostream"), py::arg("extended") = true)
      .def("fromWKT", &GeomPoint::fromWKT)
      .def("toWKT", &GeomPoint::toWKT, py::arg("extended") = true)
      .def("fromHEX", &GeomPoint::fromHEX)
      .def("toHEX", &GeomPoint::toHEX, py::arg("ostream"), py::arg("extended") = true);
}
