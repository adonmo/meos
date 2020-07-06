#pragma once

#include "chrono.h"
#include <meos/types/box/STBox.hpp>
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

  py::class_<STBox>(box_module, "STBox")
      .def(py::init<>())
      .def(py::init<double, double, double, time_point, double, double, double,
                    time_point, int, bool>(),
           py::arg("xmin"), py::arg("ymin"), py::arg("zmin"), py::arg("tmin"),
           py::arg("xmax"), py::arg("ymax"), py::arg("zmax"), py::arg("tmax"),
           py::arg("srid") = 0, py::arg("geodetic") = false)
      .def(py::init<double, double, double, string &, double, double, double,
                    string &, int, bool>(),
           py::arg("xmin"), py::arg("ymin"), py::arg("zmin"), py::arg("tmin"),
           py::arg("xmax"), py::arg("ymax"), py::arg("zmax"), py::arg("tmax"),
           py::arg("srid") = 0, py::arg("geodetic") = false)
      .def(
          py::init<double, double, double, double, double, double, int, bool>(),
          py::arg("xmin"), py::arg("ymin"), py::arg("zmin"), py::arg("xmax"),
          py::arg("ymax"), py::arg("zmax"), py::arg("srid") = 0,
          py::arg("geodetic") = false)
      .def(py::init<double, double, time_point, double, double, time_point,
                    int>(),
           py::arg("xmin"), py::arg("ymin"), py::arg("tmin"), py::arg("xmax"),
           py::arg("ymax"), py::arg("tmax"), py::arg("srid") = 0)
      .def(py::init<double, double, string &, double, double, string &, int>(),
           py::arg("xmin"), py::arg("ymin"), py::arg("tmin"), py::arg("xmax"),
           py::arg("ymax"), py::arg("tmax"), py::arg("srid") = 0)
      .def(py::init<double, double, double, double, int>(), py::arg("xmin"),
           py::arg("ymin"), py::arg("xmax"), py::arg("ymax"),
           py::arg("srid") = 0)
      .def(py::init<time_point, time_point, int, bool>(), py::arg("tmin"),
           py::arg("tmax"), py::arg("srid") = 0, py::arg("geodetic") = false)
      .def(py::init<string &, string &, int, bool>(), py::arg("tmin"),
           py::arg("tmax"), py::arg("srid") = 0, py::arg("geodetic") = false)
      .def(py::init<string &>(), py::arg("serialized"))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def(py::self < py::self)
      .def(py::self <= py::self)
      .def(py::self > py::self)
      .def(py::self >= py::self)
      .def("__str__",
           [](STBox const &self) {
             std::ostringstream s;
             s << self;
             return s.str();
           })
      .def_property_readonly("xmin", &STBox::xmin)
      .def_property_readonly("ymin", &STBox::ymin)
      .def_property_readonly("zmin", &STBox::zmin)
      .def_property_readonly("tmin", &STBox::tmin)
      .def_property_readonly("xmax", &STBox::xmax)
      .def_property_readonly("ymax", &STBox::ymax)
      .def_property_readonly("zmax", &STBox::zmax)
      .def_property_readonly("tmax", &STBox::tmax)
      .def_property_readonly("srid", &STBox::srid)
      .def_property_readonly("geodetic", &STBox::geodetic);
}
