#pragma once

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantSet.hpp>
#include <meos/types/temporal/TemporalSet.hpp>
#include <string>

#include "temporalset.hpp"

using namespace meos;
namespace py = pybind11;

template <typename BaseType> using py_tinstantset
    = py::class_<TInstantSet<BaseType>, TemporalSet<BaseType>>;

template <typename BaseType>
py_tinstantset<BaseType> _def_tinstantset_class_basic(py::module &m,
                                                      std::string const &base_type_name) {
  return py_tinstantset<BaseType>(m, ("T" + base_type_name + "InstSet").c_str())
      .def(py::init<std::set<TInstant<BaseType>> &>(), py::arg("instants"))
      .def(py::init<std::set<std::string> &>(), py::arg("instants"))
      .def(py::init<std::string &>(), py::arg("serialized"))
      .def(py::self == py::self, py::arg("other"))
      .def(py::self != py::self, py::arg("other"))
      .def(py::self < py::self, py::arg("other"))
      .def(py::self <= py::self, py::arg("other"))
      .def(py::self > py::self, py::arg("other"))
      .def(py::self >= py::self, py::arg("other"))
      .def("__str__", &to_ostream<TInstantSet<BaseType>>)
      .def("__repr__", &to_ostream<TInstantSet<BaseType>>)
      .def("compare", &TInstantSet<BaseType>::compare, py::arg("other"))
      .def_property_readonly("duration", &TInstantSet<BaseType>::duration)
      .def_property_readonly("timespan", &TInstantSet<BaseType>::timespan)
      .def_property_readonly("getValues", &TInstantSet<BaseType>::getValues)
      .def_property_readonly("getTime", &TInstantSet<BaseType>::getTime)
      .def_property_readonly("period", &TInstantSet<BaseType>::period)
      .def("shift", &TInstantSet<BaseType>::shift, py::arg("timedelta"))
      .def("intersectsTimestamp", &TInstantSet<BaseType>::intersectsTimestamp, py::arg("datetime"))
      .def("intersectsPeriod", &TInstantSet<BaseType>::intersectsPeriod, py::arg("period"));
}

template <typename BaseType>
void _def_tinstantset_class_specializations(py_tinstantset<BaseType> &c,
                                            std::string const &base_type_name) {
  // No specializations by default
}

template <> void _def_tinstantset_class_specializations(py_tinstantset<GeomPoint> &c,
                                                        std::string const &base_type_name) {
  c.def(py::init<std::set<TInstant<GeomPoint>> &, int>(), py::arg("instants"), py::arg("srid"))
      .def(py::init<std::set<std::string> &, int>(), py::arg("instants"), py::arg("srid"))
      .def(py::init<std::string, int>(), py::arg("serialized"), py::arg("srid"));
}

template <typename BaseType>
void def_tinstantset_class(py::module &m, std::string const &base_type_name) {
  auto tinstantset_class = _def_tinstantset_class_basic<BaseType>(m, base_type_name);
  _def_tinstantset_class_specializations<BaseType>(tinstantset_class, base_type_name);
}
