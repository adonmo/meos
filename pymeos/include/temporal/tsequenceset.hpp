#pragma once

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <meos/types/temporal/Interpolation.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TSequence.hpp>
#include <meos/types/temporal/TSequenceSet.hpp>
#include <string>

#include "common.hpp"

using namespace meos;
namespace py = pybind11;

template <typename BaseType> using py_tsequenceset
    = py::class_<TSequenceSet<BaseType>, Temporal<BaseType>,
                 TemporalComparators<TSequenceSet<BaseType>>,
                 TInstantFunctions<TSequenceSet<BaseType>, TInstant<BaseType>, BaseType>>;

template <typename BaseType>
py_tsequenceset<BaseType> _def_tsequenceset_class_basic(py::module &m,
                                                        std::string const &base_type_name) {
  def_comparator<TemporalComparators<TSequenceSet<BaseType>>>(m, "SeqSet", base_type_name);
  def_tinstant_functions<TInstantFunctions<TSequenceSet<BaseType>, TInstant<BaseType>, BaseType>>(
      m, "SeqSet", base_type_name);
  return py_tsequenceset<BaseType>(m, ("T" + base_type_name + "SeqSet").c_str())
      .def(py::init<std::set<TSequence<BaseType>> &, Interpolation>(), py::arg("sequences"),
           py::arg("interpolation") = default_interp_v<BaseType>)
      .def(py::init<std::set<std::string> &, Interpolation>(), py::arg("sequences"),
           py::arg("interpolation") = default_interp_v<BaseType>)
      .def(py::init<std::string>(), py::arg("serialized"))
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
      .def_property_readonly("interpolation", &TSequenceSet<BaseType>::interpolation)
      .def_property_readonly("sequences", &TSequenceSet<BaseType>::sequences)
      .def_property_readonly("numSequences", &TSequenceSet<BaseType>::numSequences)
      .def_property_readonly("startSequence", &TSequenceSet<BaseType>::startSequence)
      .def_property_readonly("endSequence", &TSequenceSet<BaseType>::endSequence)
      .def("sequenceN", &TSequenceSet<BaseType>::sequenceN, py::arg("n"))
      .def_property_readonly("instants", &TSequenceSet<BaseType>::instants)
      .def_property_readonly("timespan", &TSequenceSet<BaseType>::timespan)
      .def_property_readonly("getValues", &TSequenceSet<BaseType>::getValues)
      .def_property_readonly("timestamps", &TSequenceSet<BaseType>::timestamps)
      .def_property_readonly("getTime", &TSequenceSet<BaseType>::getTime)
      .def_property_readonly("period", &TSequenceSet<BaseType>::period)
      .def("shift", &TSequenceSet<BaseType>::shift, py::arg("timedelta"))
      .def("intersectsTimestamp", &TSequenceSet<BaseType>::intersectsTimestamp, py::arg("datetime"))
      .def("intersectsPeriod", &TSequenceSet<BaseType>::intersectsPeriod, py::arg("period"));
}

template <typename BaseType>
void _def_tsequenceset_class_specializations(py_tsequenceset<BaseType> &c,
                                             std::string const &base_type_name) {
  // No specializations by default
}

template <> void _def_tsequenceset_class_specializations(py_tsequenceset<GeomPoint> &c,
                                                         std::string const &base_type_name) {
  c.def(py::init<std::set<TSequence<GeomPoint>> &, int, Interpolation>(), py::arg("sequences"),
        py::arg("srid"), py::arg("interpolation") = default_interp_v<GeomPoint>)
      .def(py::init<std::set<std::string> &, int, Interpolation>(), py::arg("sequences"),
           py::arg("srid"), py::arg("interpolation") = default_interp_v<GeomPoint>)
      .def(py::init<std::string, int>(), py::arg("serialized"), py::arg("srid"));
}

template <typename BaseType>
void def_tsequenceset_class(py::module &m, std::string const &base_type_name) {
  auto tsequenceset_class = _def_tsequenceset_class_basic<BaseType>(m, base_type_name);
  _def_tsequenceset_class_specializations<BaseType>(tsequenceset_class, base_type_name);
}
