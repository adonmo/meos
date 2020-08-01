#pragma once

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <meos/types/box/STBox.hpp>
#include <meos/types/box/TBox.hpp>
#include <sstream>
#include <string>

#include "chrono.h"
#include "common.hpp"

using namespace meos;
namespace py = pybind11;

void def_box_module(py::module &m) {
  py::module box_module = m.def_submodule("box",
                                          "This module defines MobilityDB's box types: "
                                          "TBox and STBox");

  py::class_<TBox>(box_module, "TBox")
      .def(py::init<>())
      .def(py::init<double, time_point, double, time_point>(), py::arg("xmin"), py::arg("tmin"),
           py::arg("xmax"), py::arg("tmax"))
      .def(py::init<double, double>(), py::arg("xmin"), py::arg("xmax"))
      .def(py::init<time_point, time_point>(), py::arg("tmin"), py::arg("tmax"))
      .def(py::init<double, std::string const &, double, std::string const &>(), py::arg("xmin"),
           py::arg("tmin"), py::arg("xmax"), py::arg("tmax"))
      .def(py::init<std::string const &, std::string const &, std::string const &,
                    std::string const &>(),
           py::arg("xmin"), py::arg("tmin"), py::arg("xmax"), py::arg("tmax"))
      .def(py::init<std::string const &>(), py::arg("serialized"))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def(py::self < py::self)
      .def(py::self <= py::self)
      .def(py::self > py::self)
      .def(py::self >= py::self)
      .def("__str__", &to_ostream<TBox>)
      .def("__repr__", &to_ostream<TBox>)
      .def_property_readonly("xmin", &TBox::xmin)
      .def_property_readonly("tmin", &TBox::tmin)
      .def_property_readonly("xmax", &TBox::xmax)
      .def_property_readonly("tmax", &TBox::tmax);

  py::class_<STBox>(box_module, "STBox")
      .def(py::init<>())
      .def(py::init<double, double, double, time_point, double, double, double, time_point, int,
                    bool>(),
           py::arg("xmin"), py::arg("ymin"), py::arg("zmin"), py::arg("tmin"), py::arg("xmax"),
           py::arg("ymax"), py::arg("zmax"), py::arg("tmax"), py::arg("srid") = 0,
           py::arg("geodetic") = false)
      .def(py::init<double, double, double, std::string &, double, double, double, std::string &,
                    int, bool>(),
           py::arg("xmin"), py::arg("ymin"), py::arg("zmin"), py::arg("tmin"), py::arg("xmax"),
           py::arg("ymax"), py::arg("zmax"), py::arg("tmax"), py::arg("srid") = 0,
           py::arg("geodetic") = false)
      .def(py::init<double, double, double, double, double, double, int, bool>(), py::arg("xmin"),
           py::arg("ymin"), py::arg("zmin"), py::arg("xmax"), py::arg("ymax"), py::arg("zmax"),
           py::arg("srid") = 0, py::arg("geodetic") = false)
      .def(py::init<double, double, time_point, double, double, time_point, int>(), py::arg("xmin"),
           py::arg("ymin"), py::arg("tmin"), py::arg("xmax"), py::arg("ymax"), py::arg("tmax"),
           py::arg("srid") = 0)
      .def(py::init<double, double, std::string &, double, double, std::string &, int>(),
           py::arg("xmin"), py::arg("ymin"), py::arg("tmin"), py::arg("xmax"), py::arg("ymax"),
           py::arg("tmax"), py::arg("srid") = 0)
      .def(py::init<double, double, double, double, int>(), py::arg("xmin"), py::arg("ymin"),
           py::arg("xmax"), py::arg("ymax"), py::arg("srid") = 0)
      .def(py::init<time_point, time_point, int, bool>(), py::arg("tmin"), py::arg("tmax"),
           py::arg("srid") = 0, py::arg("geodetic") = false)
      .def(py::init<std::string &, std::string &, int, bool>(), py::arg("tmin"), py::arg("tmax"),
           py::arg("srid") = 0, py::arg("geodetic") = false)
      .def(py::init<std::string &>(), py::arg("serialized"))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def(py::self < py::self)
      .def(py::self <= py::self)
      .def(py::self > py::self)
      .def(py::self >= py::self)
      .def("__str__", &to_ostream<STBox>)
      .def("__repr__", &to_ostream<STBox>)
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
