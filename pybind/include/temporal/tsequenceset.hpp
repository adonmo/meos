#pragma once

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <meos/types/temporal/Interpolation.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TSequence.hpp>
#include <meos/types/temporal/TSequenceSet.hpp>

#include "common.hpp"

namespace py = pybind11;

template <typename BaseType>
void def_tsequenceset_class(py::module &m, std::string const &typesuffix) {
  def_comparator<TemporalComparators<TSequenceSet<BaseType>>>(m, "TSequenceSet",
                                                              typesuffix);
  def_tinstant_functions<
      TInstantFunctions<TSequenceSet<BaseType>, TInstant<BaseType>, BaseType>>(
      m, "TSequenceSet", typesuffix);
  py::class_<
      TSequenceSet<BaseType>, Temporal<BaseType>,
      TemporalComparators<TSequenceSet<BaseType>>,
      TInstantFunctions<TSequenceSet<BaseType>, TInstant<BaseType>, BaseType>>(
      m, ("TSequenceSet" + typesuffix).c_str())
      .def(py::init<set<TSequence<BaseType>> &, Interpolation>(),
           py::arg("sequences"),
           py::arg("interpolation") = default_interp_v<BaseType>)
      .def(py::init<set<string> &, Interpolation>(), py::arg("sequences"),
           py::arg("interpolation") = default_interp_v<BaseType>)
      .def(py::init<string>(), py::arg("serialized"))
      .def(py::self == py::self, py::arg("other"))
      .def(py::self != py::self, py::arg("other"))
      .def(py::self < py::self, py::arg("other"))
      .def(py::self <= py::self, py::arg("other"))
      .def(py::self > py::self, py::arg("other"))
      .def(py::self >= py::self, py::arg("other"))
      .def("__str__", &to_ostream<TSequenceSet<BaseType>>)
      .def("__repr__", &to_ostream<TSequenceSet<BaseType>>)
      .def("compare", &TSequenceSet<BaseType>::compare, py::arg("other"))
      .def_property_readonly("duration", &TSequenceSet<BaseType>::duration)
      .def_property_readonly("interpolation",
                             &TSequenceSet<BaseType>::interpolation)
      .def_property_readonly("sequences", &TSequenceSet<BaseType>::sequences)
      .def_property_readonly("numSequences",
                             &TSequenceSet<BaseType>::numSequences)
      .def_property_readonly("startSequence",
                             &TSequenceSet<BaseType>::startSequence)
      .def_property_readonly("endSequence",
                             &TSequenceSet<BaseType>::endSequence)
      .def("sequenceN", &TSequenceSet<BaseType>::sequenceN, py::arg("n"))
      .def_property_readonly("instants", &TSequenceSet<BaseType>::instants)
      .def_property_readonly("timespan", &TSequenceSet<BaseType>::timespan)
      .def_property_readonly("getValues", &TSequenceSet<BaseType>::getValues)
      .def_property_readonly("timestamps", &TSequenceSet<BaseType>::timestamps)
      .def_property_readonly("getTime", &TSequenceSet<BaseType>::getTime)
      .def_property_readonly("period", &TSequenceSet<BaseType>::period)
      .def("shift", &TSequenceSet<BaseType>::shift, py::arg("timedelta"))
      .def("intersectsTimestamp", &TSequenceSet<BaseType>::intersectsTimestamp,
           py::arg("datetime"))
      .def("intersectsPeriod", &TSequenceSet<BaseType>::intersectsPeriod,
           py::arg("period"));
}