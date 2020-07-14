#pragma once

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <meos/types/temporal/Interpolation.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TSequence.hpp>

#include "common.hpp"

namespace py = pybind11;

template <typename BaseType>
void def_tsequence_class(py::module &m, std::string const &typesuffix) {
  def_comparator<TemporalComparators<TSequence<BaseType>>>(m, "TSequence",
                                                           typesuffix);
  def_tinstant_functions<
      TInstantFunctions<TSequence<BaseType>, TInstant<BaseType>, BaseType>>(
      m, "TSequence", typesuffix);
  py::class_<
      TSequence<BaseType>, Temporal<BaseType>,
      TemporalComparators<TSequence<BaseType>>,
      TInstantFunctions<TSequence<BaseType>, TInstant<BaseType>, BaseType>>(
      m, ("TSequence" + typesuffix).c_str())
      .def(py::init<set<TInstant<BaseType>> &, bool, bool, Interpolation>(),
           py::arg("instants"), py::arg("lower_inc") = true,
           py::arg("upper_inc") = false,
           py::arg("interpolation") = default_interp_v<BaseType>)
      .def(py::init<set<string> &, bool, bool, Interpolation>(),
           py::arg("instants"), py::arg("lower_inc") = true,
           py::arg("upper_inc") = false,
           py::arg("interpolation") = default_interp_v<BaseType>)
      .def(py::init<string>(), py::arg("serialized"))
      .def(py::self == py::self, py::arg("other"))
      .def(py::self != py::self, py::arg("other"))
      .def(py::self < py::self, py::arg("other"))
      .def(py::self <= py::self, py::arg("other"))
      .def(py::self > py::self, py::arg("other"))
      .def(py::self >= py::self, py::arg("other"))
      .def("__str__", &to_ostream<TSequence<BaseType>>)
      .def("__repr__", &to_ostream<TSequence<BaseType>>)
      .def("__hash__",
           [](TSequence<BaseType> const &self) {
             // TODO is there a better way?
             std::ostringstream s;
             s << self;
             return py::hash(py::make_tuple(s.str()));
           })
      .def("compare", &TSequence<BaseType>::compare, py::arg("other"))
      .def_property_readonly("lower_inc", &TSequence<BaseType>::lower_inc)
      .def_property_readonly("upper_inc", &TSequence<BaseType>::upper_inc)
      .def_property_readonly("duration", &TSequence<BaseType>::duration)
      .def_property_readonly("instants", &TSequence<BaseType>::instants)
      .def_property_readonly("interpolation",
                             &TSequence<BaseType>::interpolation)
      .def_property_readonly("timespan", &TSequence<BaseType>::timespan)
      .def_property_readonly("getValues", &TSequence<BaseType>::getValues)
      .def_property_readonly("timestamps", &TSequence<BaseType>::timestamps)
      .def_property_readonly("getTime", &TSequence<BaseType>::getTime)
      .def_property_readonly("period", &TSequence<BaseType>::period)
      .def("shift", &TSequence<BaseType>::shift, py::arg("timedelta"))
      .def("intersectsTimestamp", &TSequence<BaseType>::intersectsTimestamp,
           py::arg("datetime"))
      .def("intersectsPeriod", &TSequence<BaseType>::intersectsPeriod,
           py::arg("period"));
}