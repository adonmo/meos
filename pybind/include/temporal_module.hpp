#pragma once

#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantSet.hpp>
#include <meos/types/temporal/TSequence.hpp>
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
      .def_property_readonly("endValue", &Interface::endValue);
}

template <typename T>
void def_temporal_types(py::module &m, std::string const &typesuffix) {
  def_comparator<TemporalComparators<TInstant<T>>>(m, "TInstant", typesuffix);
  def_tinstant_functions<TInstantFunctions<TInstant<T>, TInstant<T>, T>>(
      m, "TInstant", typesuffix);
  py::class_<TInstant<T>, TemporalComparators<TInstant<T>>,
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
  py::class_<TInstantSet<T>, TemporalComparators<TInstantSet<T>>,
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
  py::class_<TSequence<T>, TemporalComparators<TSequence<T>>,
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
      .def("compare", &TSequence<T>::compare, py::arg("other"))
      .def_property_readonly("lower_inc", &TSequence<T>::lower_inc)
      .def_property_readonly("upper_inc", &TSequence<T>::upper_inc)
      .def_property_readonly("duration", &TSequence<T>::duration)
      .def_property_readonly("instants", &TSequence<T>::instants)
      .def_property_readonly("getValues", &TSequence<T>::getValues)
      .def_property_readonly("timestamps", &TSequence<T>::timestamps)
      .def_property_readonly("getTime", &TSequence<T>::getTime)
      .def_property_readonly("period", &TSequence<T>::period)
      .def("shift", &TSequence<T>::shift, py::arg("timedelta"))
      .def("intersectsTimestamp", &TSequence<T>::intersectsTimestamp,
           py::arg("datetime"))
      .def("intersectsPeriod", &TSequence<T>::intersectsPeriod,
           py::arg("period"));
}

void def_temporal_module(py::module &m) {
  py::module temporal_module = m.def_submodule(
      "temporal", "This module defines MobilityDB's temporal types: Temporal, "
                  "TInstant, TInstantSet, TSequence and TSequenceSet");

  def_temporal_types<bool>(temporal_module, "Bool");
  def_temporal_types<int>(temporal_module, "Int");
  def_temporal_types<float>(temporal_module, "Float");
  def_temporal_types<string>(temporal_module, "Text");
  def_temporal_types<Geometry>(temporal_module, "Geom");
}
