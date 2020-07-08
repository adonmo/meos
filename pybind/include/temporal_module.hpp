#pragma once

#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantSet.hpp>
#include <meos/types/temporal/TSequence.hpp>
#include <meos/types/temporal/TSequenceSet.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/types/temporal/TemporalDuration.hpp>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

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

template <typename T>
void def_temporal_types(py::module &m, std::string const &typesuffix) {
  py::class_<Temporal<T>>(m, ("Temporal" + typesuffix).c_str())
      .def_property_readonly("minValue", &Temporal<T>::minValue)
      .def_property_readonly("maxValue", &Temporal<T>::maxValue)
      .def_property_readonly("numTimestamps", &Temporal<T>::numTimestamps)
      .def_property_readonly("startTimestamp", &Temporal<T>::startTimestamp)
      .def_property_readonly("endTimestamp", &Temporal<T>::endTimestamp)
      .def("timestampN", &Temporal<T>::timestampN, py::arg("n"))
      .def("intersectsTimestampSet", &Temporal<T>::intersectsTimestampSet,
           py::arg("timestampset"))
      .def("intersectsPeriodSet", &Temporal<T>::intersectsPeriodSet,
           py::arg("periodset"));

  def_comparator<TemporalComparators<TInstant<T>>>(m, "TInstant", typesuffix);
  def_tinstant_functions<TInstantFunctions<TInstant<T>, TInstant<T>, T>>(
      m, "TInstant", typesuffix);
  py::class_<TInstant<T>, Temporal<T>, TemporalComparators<TInstant<T>>,
             TInstantFunctions<TInstant<T>, TInstant<T>, T>>(
      m, ("TInstant" + typesuffix).c_str())
      .def(py::init<T, time_point>(), py::arg("value"), py::arg("timestamp"))
      .def(py::init<pair<T, time_point>>(), py::arg("instant"))
      .def(py::init<string, string>(), py::arg("value"), py::arg("timestamp"))
      .def(py::init<pair<string, string>>(), py::arg("instant"))
      .def(py::init<string>(), py::arg("serialized"))
      .def(py::self == py::self, py::arg("other"))
      .def(py::self != py::self, py::arg("other"))
      .def(py::self < py::self, py::arg("other"))
      .def(py::self <= py::self, py::arg("other"))
      .def(py::self > py::self, py::arg("other"))
      .def(py::self >= py::self, py::arg("other"))
      .def("__str__",
           [](TInstant<T> const &self) {
             std::ostringstream s;
             s << self;
             return s.str();
           })
      .def("__hash__",
           [](TInstant<T> const &instant) {
             return py::hash(
                 py::make_tuple(instant.getValue(), instant.getTimestamp()));
           })
      .def("compare", &TInstant<T>::compare, py::arg("other"))
      .def_property_readonly("getTimestamp", &TInstant<T>::getTimestamp)
      .def_property_readonly("getValue", &TInstant<T>::getValue)
      .def_property_readonly("duration", &TInstant<T>::duration)
      .def_property_readonly("instants", &TInstant<T>::instants)
      .def_property_readonly("timespan", &TInstant<T>::timespan)
      .def_property_readonly("getValues", &TInstant<T>::getValues)
      .def_property_readonly("timestamps", &TInstant<T>::timestamps)
      .def_property_readonly("getTime", &TInstant<T>::getTime)
      .def_property_readonly("period", &TInstant<T>::period)
      .def("shift", &TInstant<T>::shift, py::arg("timedelta"))
      .def("intersectsTimestamp", &TInstant<T>::intersectsTimestamp,
           py::arg("datetime"))
      .def("intersectsPeriod", &TInstant<T>::intersectsPeriod,
           py::arg("period"));

  def_comparator<TemporalComparators<TInstantSet<T>>>(m, "TInstantSet",
                                                      typesuffix);
  def_tinstant_functions<TInstantFunctions<TInstantSet<T>, TInstant<T>, T>>(
      m, "TInstantSet", typesuffix);
  py::class_<TInstantSet<T>, Temporal<T>, TemporalComparators<TInstantSet<T>>,
             TInstantFunctions<TInstantSet<T>, TInstant<T>, T>>(
      m, ("TInstantSet" + typesuffix).c_str())
      .def(py::init<set<TInstant<T>> &>(), py::arg("instants"))
      .def(py::init<set<string> &>(), py::arg("instants"))
      .def(py::init<string>(), py::arg("serialized"))
      .def(py::self == py::self, py::arg("other"))
      .def(py::self != py::self, py::arg("other"))
      .def(py::self < py::self, py::arg("other"))
      .def(py::self <= py::self, py::arg("other"))
      .def(py::self > py::self, py::arg("other"))
      .def(py::self >= py::self, py::arg("other"))
      .def("__str__",
           [](TInstantSet<T> const &self) {
             std::ostringstream s;
             s << self;
             return s.str();
           })
      .def("compare", &TInstantSet<T>::compare, py::arg("other"))
      .def_property_readonly("duration", &TInstantSet<T>::duration)
      .def_property_readonly("instants", &TInstantSet<T>::instants)
      .def_property_readonly("timespan", &TInstantSet<T>::timespan)
      .def_property_readonly("getValues", &TInstantSet<T>::getValues)
      .def_property_readonly("timestamps", &TInstantSet<T>::timestamps)
      .def_property_readonly("getTime", &TInstantSet<T>::getTime)
      .def_property_readonly("period", &TInstantSet<T>::period)
      .def("shift", &TInstantSet<T>::shift, py::arg("timedelta"))
      .def("intersectsTimestamp", &TInstantSet<T>::intersectsTimestamp,
           py::arg("datetime"))
      .def("intersectsPeriod", &TInstantSet<T>::intersectsPeriod,
           py::arg("period"));

  def_comparator<TemporalComparators<TSequence<T>>>(m, "TSequence", typesuffix);
  def_tinstant_functions<TInstantFunctions<TSequence<T>, TInstant<T>, T>>(
      m, "TSequence", typesuffix);
  py::class_<TSequence<T>, Temporal<T>, TemporalComparators<TSequence<T>>,
             TInstantFunctions<TSequence<T>, TInstant<T>, T>>(
      m, ("TSequence" + typesuffix).c_str())
      .def(py::init<set<TInstant<T>> &, bool, bool>(), py::arg("instants"),
           py::arg("lower_inc") = true, py::arg("upper_inc") = false)
      .def(py::init<set<string> &, bool, bool>(), py::arg("instants"),
           py::arg("lower_inc") = true, py::arg("upper_inc") = false)
      .def(py::init<string>(), py::arg("serialized"))
      .def(py::self == py::self, py::arg("other"))
      .def(py::self != py::self, py::arg("other"))
      .def(py::self < py::self, py::arg("other"))
      .def(py::self <= py::self, py::arg("other"))
      .def(py::self > py::self, py::arg("other"))
      .def(py::self >= py::self, py::arg("other"))
      .def("__str__",
           [](TSequence<T> const &self) {
             std::ostringstream s;
             s << self;
             return s.str();
           })
      .def("__hash__",
           [](TSequence<T> const &self) {
             // TODO is there a better way?
             std::ostringstream s;
             s << self;
             return py::hash(py::make_tuple(s.str()));
           })
      .def("compare", &TSequence<T>::compare, py::arg("other"))
      .def_property_readonly("lower_inc", &TSequence<T>::lower_inc)
      .def_property_readonly("upper_inc", &TSequence<T>::upper_inc)
      .def_property_readonly("duration", &TSequence<T>::duration)
      .def_property_readonly("instants", &TSequence<T>::instants)
      .def_property_readonly("timespan", &TSequence<T>::timespan)
      .def_property_readonly("getValues", &TSequence<T>::getValues)
      .def_property_readonly("timestamps", &TSequence<T>::timestamps)
      .def_property_readonly("getTime", &TSequence<T>::getTime)
      .def_property_readonly("period", &TSequence<T>::period)
      .def("shift", &TSequence<T>::shift, py::arg("timedelta"))
      .def("intersectsTimestamp", &TSequence<T>::intersectsTimestamp,
           py::arg("datetime"))
      .def("intersectsPeriod", &TSequence<T>::intersectsPeriod,
           py::arg("period"));

  def_comparator<TemporalComparators<TSequenceSet<T>>>(m, "TSequenceSet",
                                                       typesuffix);
  def_tinstant_functions<TInstantFunctions<TSequenceSet<T>, TInstant<T>, T>>(
      m, "TSequenceSet", typesuffix);
  py::class_<TSequenceSet<T>, Temporal<T>, TemporalComparators<TSequenceSet<T>>,
             TInstantFunctions<TSequenceSet<T>, TInstant<T>, T>>(
      m, ("TSequenceSet" + typesuffix).c_str())
      .def(py::init<set<TSequence<T>> &>(), py::arg("sequences"))
      .def(py::init<set<string> &>(), py::arg("sequences"))
      .def(py::init<string>(), py::arg("serialized"))
      .def(py::self == py::self, py::arg("other"))
      .def(py::self != py::self, py::arg("other"))
      .def(py::self < py::self, py::arg("other"))
      .def(py::self <= py::self, py::arg("other"))
      .def(py::self > py::self, py::arg("other"))
      .def(py::self >= py::self, py::arg("other"))
      .def("__str__",
           [](TSequenceSet<T> const &self) {
             std::ostringstream s;
             s << self;
             return s.str();
           })
      .def("compare", &TSequenceSet<T>::compare, py::arg("other"))
      .def_property_readonly("duration", &TSequenceSet<T>::duration)
      .def_property_readonly("sequences", &TSequenceSet<T>::sequences)
      .def_property_readonly("numSequences", &TSequenceSet<T>::numSequences)
      .def_property_readonly("startSequence", &TSequenceSet<T>::startSequence)
      .def_property_readonly("endSequence", &TSequenceSet<T>::endSequence)
      .def("sequenceN", &TSequenceSet<T>::sequenceN, py::arg("n"))
      .def_property_readonly("instants", &TSequenceSet<T>::instants)
      .def_property_readonly("timespan", &TSequenceSet<T>::timespan)
      .def_property_readonly("getValues", &TSequenceSet<T>::getValues)
      .def_property_readonly("timestamps", &TSequenceSet<T>::timestamps)
      .def_property_readonly("getTime", &TSequenceSet<T>::getTime)
      .def_property_readonly("period", &TSequenceSet<T>::period)
      .def("shift", &TSequenceSet<T>::shift, py::arg("timedelta"))
      .def("intersectsTimestamp", &TSequenceSet<T>::intersectsTimestamp,
           py::arg("datetime"))
      .def("intersectsPeriod", &TSequenceSet<T>::intersectsPeriod,
           py::arg("period"));
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

  def_temporal_types<bool>(temporal_module, "Bool");
  def_temporal_types<int>(temporal_module, "Int");
  def_temporal_types<float>(temporal_module, "Float");
  def_temporal_types<string>(temporal_module, "Text");
  def_temporal_types<Geometry>(temporal_module, "Geom");
}
