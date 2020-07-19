#pragma once

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantSet.hpp>

#include "common.hpp"

namespace py = pybind11;

template <typename BaseType>
using py_tinstantset = py::class_<
    TInstantSet<BaseType>, Temporal<BaseType>,
    TemporalComparators<TInstantSet<BaseType>>,
    TInstantFunctions<TInstantSet<BaseType>, TInstant<BaseType>, BaseType>>;

template <typename BaseType>
py_tinstantset<BaseType>
_def_tinstantset_class_basic(py::module &m, std::string const &typesuffix) {
  def_comparator<TemporalComparators<TInstantSet<BaseType>>>(m, "TInstantSet",
                                                             typesuffix);
  def_tinstant_functions<
      TInstantFunctions<TInstantSet<BaseType>, TInstant<BaseType>, BaseType>>(
      m, "TInstantSet", typesuffix);
  return py_tinstantset<BaseType>(m, ("TInstantSet" + typesuffix).c_str())
      .def(py::init<set<TInstant<BaseType>> &>(), py::arg("instants"))
      .def(py::init<set<string> &>(), py::arg("instants"))
      .def(py::init<string &>(), py::arg("serialized"))
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
      .def_property_readonly("instants", &TInstantSet<BaseType>::instants)
      .def_property_readonly("timespan", &TInstantSet<BaseType>::timespan)
      .def_property_readonly("getValues", &TInstantSet<BaseType>::getValues)
      .def_property_readonly("timestamps", &TInstantSet<BaseType>::timestamps)
      .def_property_readonly("getTime", &TInstantSet<BaseType>::getTime)
      .def_property_readonly("period", &TInstantSet<BaseType>::period)
      .def("shift", &TInstantSet<BaseType>::shift, py::arg("timedelta"))
      .def("intersectsTimestamp", &TInstantSet<BaseType>::intersectsTimestamp,
           py::arg("datetime"))
      .def("intersectsPeriod", &TInstantSet<BaseType>::intersectsPeriod,
           py::arg("period"));
}

template <typename BaseType>
void _def_tinstantset_class_specializations(py_tinstantset<BaseType> &c,
                                            std::string const &typesuffix) {
  // No specializations by default
}

template <>
void _def_tinstantset_class_specializations(py_tinstantset<Geometry> &c,
                                            std::string const &typesuffix) {
  c.def(py::init<set<TInstant<Geometry>> &, int>(), py::arg("instants"),
        py::arg("srid"))
      .def(py::init<set<string> &, int>(), py::arg("instants"), py::arg("srid"))
      .def(py::init<string, int>(), py::arg("serialized"), py::arg("srid"));
}

template <typename BaseType>
void def_tinstantset_class(py::module &m, std::string const &typesuffix) {
  auto tinstantset_class =
      _def_tinstantset_class_basic<BaseType>(m, typesuffix);
  _def_tinstantset_class_specializations<BaseType>(tinstantset_class,
                                                   typesuffix);
}
