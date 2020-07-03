#pragma once

#include "chrono.h"
#include <meos/types/box/TBox.hpp>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <sstream>

namespace py = pybind11;

void def_box_module(py::module &m) {
  py::module box_module =
      m.def_submodule("box", "This module defines MobilityDB's box types: "
                             "TBox and STBox");

  py::class_<TBox>(box_module, "TBox")
      .def(py::init<>())
      .def(py::init<double, time_point, double, time_point>(), py::arg("xmin"),
           py::arg("tmin"), py::arg("xmax"), py::arg("tmax"))
      .def(py::init<double, double>(), py::arg("xmin"), py::arg("xmax"))
      .def(py::init<time_point, time_point>(), py::arg("tmin"), py::arg("tmax"))
      .def(py::init<double, string const &, double, string const &>(),
           py::arg("xmin"), py::arg("tmin"), py::arg("xmax"), py::arg("tmax"))
      .def(py::init<string const &, string const &, string const &,
                    string const &>(),
           py::arg("xmin"), py::arg("tmin"), py::arg("xmax"), py::arg("tmax"))
      .def(py::init<string const &>(), py::arg("serialized"))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def(py::self < py::self)
      .def(py::self <= py::self)
      .def(py::self > py::self)
      .def(py::self >= py::self)
      .def("__str__",
           [](TBox const &self) {
             std::ostringstream s;
             s << self;
             return s.str();
           })
      .def_property_readonly("xmin", &TBox::xmin)
      .def_property_readonly("tmin", &TBox::tmin)
      .def_property_readonly("xmax", &TBox::xmax)
      .def_property_readonly("tmax", &TBox::tmax);
}
