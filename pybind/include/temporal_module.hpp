#pragma once

#include <type_traits>

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <meos/types/temporal/Interpolation.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantSet.hpp>
#include <meos/types/temporal/TSequence.hpp>
#include <meos/types/temporal/TSequenceSet.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/types/temporal/TemporalDuration.hpp>

#include "common.hpp"

namespace py = pybind11;

template <class Interface>
void def_comparator(const pybind11::module &m,
                    std::string const &temporal_type_suffix,
                    std::string const &base_type_suffix) {
  py::class_<Interface>(
      m, ("TemporalComparators" + temporal_type_suffix + base_type_suffix)
             .c_str());
}

template <class Interface>
void def_tinstant_functions(const pybind11::module &m,
                            std::string const &temporal_type_suffix,
                            std::string const &base_type_suffix) {
  py::class_<Interface>(
      m,
      ("TInstantFunctions" + temporal_type_suffix + base_type_suffix).c_str())
      .def_property_readonly("numInstants", &Interface::numInstants)
      .def_property_readonly("startInstant", &Interface::startInstant)
      .def_property_readonly("endInstant", &Interface::endInstant)
      .def("instantN", &Interface::instantN, py::arg("n"))
      .def_property_readonly("startValue", &Interface::startValue)
      .def_property_readonly("endValue", &Interface::endValue)
      .def("valueN", &Interface::valueN, py::arg("n"));
}

template <typename BaseType>
using py_temporal = py::class_<
    Temporal<BaseType>,
    conditional_t<is_same<BaseType, Geometry>::value, SRIDMembers, Empty>>;

template <typename BaseType>
void def_temporal_class(py::module &m, std::string const &typesuffix) {
  py_temporal<BaseType>(m, ("Temporal" + typesuffix).c_str())
      .def_property_readonly("minValue", &Temporal<BaseType>::minValue)
      .def_property_readonly("maxValue", &Temporal<BaseType>::maxValue)
      .def_property_readonly("numTimestamps",
                             &Temporal<BaseType>::numTimestamps)
      .def_property_readonly("startTimestamp",
                             &Temporal<BaseType>::startTimestamp)
      .def_property_readonly("endTimestamp", &Temporal<BaseType>::endTimestamp)
      .def("timestampN", &Temporal<BaseType>::timestampN, py::arg("n"))
      .def("intersectsTimestampSet",
           &Temporal<BaseType>::intersectsTimestampSet, py::arg("timestampset"))
      .def("intersectsPeriodSet", &Temporal<BaseType>::intersectsPeriodSet,
           py::arg("periodset"));
}

template <typename BaseType>
using py_tinstant = py::class_<
    TInstant<BaseType>, Temporal<BaseType>,
    TemporalComparators<TInstant<BaseType>>,
    TInstantFunctions<TInstant<BaseType>, TInstant<BaseType>, BaseType>>;

template <typename BaseType>
py_tinstant<BaseType> _def_tinstant_class_basic(py::module &m,
                                                std::string const &typesuffix) {
  def_comparator<TemporalComparators<TInstant<BaseType>>>(m, "TInstant",
                                                          typesuffix);
  def_tinstant_functions<
      TInstantFunctions<TInstant<BaseType>, TInstant<BaseType>, BaseType>>(
      m, "TInstant", typesuffix);
  return py_tinstant<BaseType>(m, ("TInstant" + typesuffix).c_str())
      .def(py::init<BaseType, time_point>(), py::arg("value"),
           py::arg("timestamp"))
      .def(py::init<pair<BaseType, time_point>>(), py::arg("instant"))
      .def(py::init<string, string>(), py::arg("value"), py::arg("timestamp"))
      .def(py::init<pair<string, string>>(), py::arg("instant"))
      .def(py::init<string>(), py::arg("serialized"))
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
             return py::hash(
                 py::make_tuple(instant.getValue(), instant.getTimestamp()));
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
      .def("intersectsTimestamp", &TInstant<BaseType>::intersectsTimestamp,
           py::arg("datetime"))
      .def("intersectsPeriod", &TInstant<BaseType>::intersectsPeriod,
           py::arg("period"));
}

template <typename BaseType>
void _def_tinstant_class_specializations(py_tinstant<BaseType> &c,
                                         std::string const &typesuffix) {
  // No specializations by default
}

template <>
void _def_tinstant_class_specializations(py_tinstant<Geometry> &c,
                                         std::string const &typesuffix) {
  c.def(py::init<Geometry, time_point, int>(), py::arg("value"),
        py::arg("timestamp"), py::arg("srid"));
}

template <typename BaseType>
void def_tinstant_class(py::module &m, std::string const &typesuffix) {
  auto tinstant_class = _def_tinstant_class_basic<BaseType>(m, typesuffix);
  _def_tinstant_class_specializations<BaseType>(tinstant_class, typesuffix);
}

template <typename BaseType>
void def_tinstantset_class(py::module &m, std::string const &typesuffix) {
  def_comparator<TemporalComparators<TInstantSet<BaseType>>>(m, "TInstantSet",
                                                             typesuffix);
  def_tinstant_functions<
      TInstantFunctions<TInstantSet<BaseType>, TInstant<BaseType>, BaseType>>(
      m, "TInstantSet", typesuffix);
  py::class_<
      TInstantSet<BaseType>, Temporal<BaseType>,
      TemporalComparators<TInstantSet<BaseType>>,
      TInstantFunctions<TInstantSet<BaseType>, TInstant<BaseType>, BaseType>>(
      m, ("TInstantSet" + typesuffix).c_str())
      .def(py::init<set<TInstant<BaseType>> &>(), py::arg("instants"))
      .def(py::init<set<string> &>(), py::arg("instants"))
      .def(py::init<string>(), py::arg("serialized"))
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

template <typename BaseType>
void def_temporal_types(py::module &m, std::string const &typesuffix) {
  def_temporal_class<BaseType>(m, typesuffix);
  def_tinstant_class<BaseType>(m, typesuffix);
  def_tinstantset_class<BaseType>(m, typesuffix);
  def_tsequence_class<BaseType>(m, typesuffix);
  def_tsequenceset_class<BaseType>(m, typesuffix);
}

void def_temporal_module(py::module &m) {
  py::module temporal_module = m.def_submodule(
      "temporal", "This module defines MobilityDB's temporal types: Temporal, "
                  "TInstant, TInstantSet, TSequence and TSequenceSet");

  py::enum_<TemporalDuration>(temporal_module, "TemporalDuration")
      .value("Temporal", TemporalDuration::Temporal)
      .value("Instant", TemporalDuration::Instant)
      .value("InstantSet", TemporalDuration::InstantSet)
      .value("Sequence", TemporalDuration::Sequence)
      .value("SequenceSet", TemporalDuration::SequenceSet);

  py::enum_<Interpolation>(temporal_module, "Interpolation")
      .value("Stepwise", Interpolation::Stepwise)
      .value("Linear", Interpolation::Linear);

  py::class_<Empty>(m, "Empty");

  py::class_<SRIDMembers>(m, "SRIDMembers")
      .def_property_readonly("srid", &SRIDMembers::srid);

  def_temporal_types<bool>(temporal_module, "Bool");
  def_temporal_types<int>(temporal_module, "Int");
  def_temporal_types<float>(temporal_module, "Float");
  def_temporal_types<string>(temporal_module, "Text");
  def_temporal_types<Geometry>(temporal_module, "Geom");
}
