#pragma once

#include "chrono.h"
#include <meos/types/time/Period.hpp>
#include <meos/types/time/PeriodSet.hpp>
#include <meos/types/time/TimestampSet.hpp>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void def_time_module(py::module &m) {
  py::module time_module =
      m.def_submodule("time", "This module defines MobilityDB's time types: "
                              "Period, PeriodSet and TimestampSet");

  py::class_<Period>(time_module, "Period")
      .def(py::init<time_point, time_point, bool, bool>(), py::arg("lower"),
           py::arg("upper"), py::arg("lower_inc") = true,
           py::arg("upper_inc") = false)
      .def(py::init<string, string, bool, bool>(), py::arg("lower"),
           py::arg("upper"), py::arg("lower_inc") = true,
           py::arg("upper_inc") = false)
      .def(py::init<string>(), py::arg("serialized"))
      .def(py::self == py::self)
      .def(py::self != py::self)
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
      .def("shift", &Period::shift, py::arg("timedelta"))
      .def("overlap", &Period::overlap, py::arg("period"))
      .def("contains_timestamp", &Period::contains_timestamp,
           py::arg("timestamp"));

  py::class_<PeriodSet>(time_module, "PeriodSet")
      .def(py::init<set<Period> &>(), py::arg("periods"))
      .def(py::init<set<string> &>(), py::arg("periods"))
      .def(py::init<string>(), py::arg("serialized"))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def(py::self < py::self)
      .def(py::self <= py::self)
      .def(py::self > py::self)
      .def(py::self >= py::self)
      .def("__str__",
           [](PeriodSet const &self) {
             std::ostringstream s;
             s << self;
             return s.str();
           })
      .def_property_readonly("periods", &PeriodSet::periods)
      .def_property_readonly("period", &PeriodSet::period)
      .def_property_readonly("numPeriods", &PeriodSet::numPeriods)
      .def_property_readonly("startPeriod", &PeriodSet::startPeriod)
      .def_property_readonly("endPeriod", &PeriodSet::endPeriod)
      .def_property_readonly("timespan", &PeriodSet::timespan)
      .def_property_readonly("timestamps", &PeriodSet::timestamps)
      .def_property_readonly("numTimestamps", &PeriodSet::numTimestamps)
      .def_property_readonly("startTimestamp", &PeriodSet::startTimestamp)
      .def_property_readonly("endTimestamp", &PeriodSet::endTimestamp)
      .def("periodN", &PeriodSet::periodN, py::arg("n"))
      .def("shift", &PeriodSet::shift, py::arg("timedelta"))
      .def("timestampN", &PeriodSet::timestampN, py::arg("n"));

  py::class_<TimestampSet>(time_module, "TimestampSet")
      .def(py::init<set<time_point> &>(), py::arg("timestamps"))
      .def(py::init<set<string> &>(), py::arg("timestamps"))
      .def(py::init<string>(), py::arg("serialized"))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def(py::self < py::self)
      .def(py::self <= py::self)
      .def(py::self > py::self)
      .def(py::self >= py::self)
      .def("__str__",
           [](TimestampSet const &self) {
             std::ostringstream s;
             s << self;
             return s.str();
           })
      .def_property_readonly("periods", &TimestampSet::periods)
      .def_property_readonly("period", &TimestampSet::period)
      .def_property_readonly("numPeriods", &TimestampSet::numPeriods)
      .def_property_readonly("startPeriod", &TimestampSet::startPeriod)
      .def_property_readonly("endPeriod", &TimestampSet::endPeriod)
      .def_property_readonly("timespan", &TimestampSet::timespan)
      .def_property_readonly("timestamps", &TimestampSet::timestamps)
      .def_property_readonly("numTimestamps", &TimestampSet::numTimestamps)
      .def_property_readonly("startTimestamp", &TimestampSet::startTimestamp)
      .def_property_readonly("endTimestamp", &TimestampSet::endTimestamp)
      .def("periodN", &TimestampSet::periodN, py::arg("n"))
      .def("shift", &TimestampSet::shift, py::arg("timedelta"))
      .def("timestampN", &TimestampSet::timestampN, py::arg("n"));
}
