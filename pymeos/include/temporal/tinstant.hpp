#pragma once

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <meos/types/temporal/TInstant.hpp>
#include <string>

#include "common.hpp"

namespace py = pybind11;

template <typename BaseType> using py_tinstant
    = py::class_<TInstant<BaseType>, Temporal<BaseType>, TemporalComparators<TInstant<BaseType>>,
                 TInstantFunctions<TInstant<BaseType>, TInstant<BaseType>, BaseType>>;

template <typename BaseType>
py_tinstant<BaseType> _def_tinstant_class_basic(py::module &m, std::string const &base_type_name) {
  def_comparator<TemporalComparators<TInstant<BaseType>>>(m, "Inst", base_type_name);
  def_tinstant_functions<TInstantFunctions<TInstant<BaseType>, TInstant<BaseType>, BaseType>>(
      m, "Inst", base_type_name);
  return py_tinstant<BaseType>(m, ("T" + base_type_name + "Inst").c_str())
      .def(py::init<BaseType &, time_point>(), py::arg("value"), py::arg("timestamp"))
      .def(py::init<pair<BaseType, time_point>>(), py::arg("instant"))
      .def(py::init<string &, string &>(), py::arg("value"), py::arg("timestamp"))
      .def(py::init<pair<string, string>>(), py::arg("instant"))
      .def(py::init<string &>(), py::arg("serialized"))
      .def(py::self == py::self, py::arg("other"))
      .def(py::self != py::self, py::arg("other"))
      .def(py::self < py::self, py::arg("other"))
      .def(py::self <= py::self, py::arg("other"))
      .def(py::self > py::self, py::arg("other"))
      .def(py::self >= py::self, py::arg("other"))
      .def("__str__", &to_ostream<TInstant<BaseType>>)
      .def("__repr__", &to_ostream<TInstant<BaseType>>)
      .def("__hash__",
           [](TInstant<BaseType> const &instant) {
             return py::hash(py::make_tuple(instant.getValue(), instant.getTimestamp()));
           })
      .def("compare", &TInstant<BaseType>::compare, py::arg("other"))
      .def_property_readonly("getTimestamp", &TInstant<BaseType>::getTimestamp)
      .def_property_readonly("getValue", &TInstant<BaseType>::getValue)
      .def_property_readonly("duration", &TInstant<BaseType>::duration)
      .def_property_readonly("instants", &TInstant<BaseType>::instants)
      .def_property_readonly("timespan", &TInstant<BaseType>::timespan)
      .def_property_readonly("getValues", &TInstant<BaseType>::getValues)
      .def_property_readonly("timestamps", &TInstant<BaseType>::timestamps)
      .def_property_readonly("getTime", &TInstant<BaseType>::getTime)
      .def_property_readonly("period", &TInstant<BaseType>::period)
      .def("shift", &TInstant<BaseType>::shift, py::arg("timedelta"))
      .def("intersectsTimestamp", &TInstant<BaseType>::intersectsTimestamp, py::arg("datetime"))
      .def("intersectsPeriod", &TInstant<BaseType>::intersectsPeriod, py::arg("period"));
}

template <typename BaseType>
void _def_tinstant_class_specializations(py_tinstant<BaseType> &c,
                                         std::string const &base_type_name) {
  // No specializations by default
}

template <> void _def_tinstant_class_specializations(py_tinstant<GeomPoint> &c,
                                                     std::string const &base_type_name) {
  c.def(py::init<GeomPoint, time_point, int>(), py::arg("value"), py::arg("timestamp"),
        py::arg("srid"))
      .def(py::init<pair<GeomPoint, time_point>, int>(), py::arg("instant"), py::arg("srid"))
      .def(py::init<string, string, int>(), py::arg("value"), py::arg("timestamp"), py::arg("srid"))
      .def(py::init<pair<string, string>, int>(), py::arg("instant"), py::arg("srid"))
      .def(py::init<string, int>(), py::arg("serialized"), py::arg("srid"));
}

template <typename BaseType>
void def_tinstant_class(py::module &m, std::string const &base_type_name) {
  auto tinstant_class = _def_tinstant_class_basic<BaseType>(m, base_type_name);
  _def_tinstant_class_specializations<BaseType>(tinstant_class, base_type_name);
}
