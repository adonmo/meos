#pragma once

#include "chrono.h"
#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantSet.hpp>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void def_time_module(py::module &m) {
  py::module time_module =
      m.def_submodule("time", "This module defines MobilityDB's time types: "
                              "Period, PeriodSet and TimestampSet");

  py::class_<Period>(time_module, "Period")
      .def(py::init<time_point const, time_point const, bool const,
                    bool const>(),
           py::arg("lower"), py::arg("upper"), py::arg("lower_inc") = true,
           py::arg("upper_inc") = false)
      .def(py::init<string const, string const, bool const, bool const>(),
           py::arg("lower"), py::arg("upper"), py::arg("lower_inc") = true,
           py::arg("upper_inc") = false)
      .def(py::init<string const>(), py::arg("serialized"))
      .def(py::self == py::self)
      .def(py::self < py::self)
      .def(py::self <= py::self)
      .def(py::self > py::self)
      .def(py::self >= py::self)
      .def("__hash__",
           [](Period const &period) {
             return py::hash(py::make_tuple(period.lower(), period.upper(),
                                            period.lower_inc(),
                                            period.upper_inc()));
           })
      .def("__str__",
           [](Period const &self) {
             std::ostringstream s;
             s << self;
             return s.str();
           })
      .def_property_readonly("lower", &Period::lower)
      .def_property_readonly("upper", &Period::upper)
      .def_property_readonly("lower_inc", &Period::lower_inc)
      .def_property_readonly("upper_inc", &Period::upper_inc)
      .def_property_readonly("timespan", &Period::timespan)
      .def("shift", &Period::shift)
      .def("overlap", &Period::overlap)
      .def("contains_timestamp", &Period::contains_timestamp);

  py::class_<PeriodSet>(time_module, "PeriodSet")
      .def(py::init<set<Period> &>())
      .def("periods", &PeriodSet::periods);
}
