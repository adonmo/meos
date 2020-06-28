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
      .def("numInstants", &Interface::numInstants)
      .def("startInstant", &Interface::startInstant)
      .def("endInstant", &Interface::endInstant)
      .def("instantN", &Interface::instantN)
      .def("startValue", &Interface::startValue)
      .def("endValue", &Interface::endValue);
}

template <typename T>
void declare_temporal_types(py::module &m, std::string const &typesuffix) {
  def_comparator<TemporalComparators<TInstant<T>>>(m, "TInstant", typesuffix);
  def_tinstant_functions<TInstantFunctions<TInstant<T>, TInstant<T>, T>>(
      m, "TInstant", typesuffix);
  py::class_<TInstant<T>, TemporalComparators<TInstant<T>>,
             TInstantFunctions<TInstant<T>, TInstant<T>, T>>(
      m, ("TInstant" + typesuffix).c_str())
      .def(py::init<T, time_point>())
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def(py::self < py::self)
      .def(py::self <= py::self)
      .def(py::self > py::self)
      .def(py::self >= py::self)
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
      .def("getTimestamp", &TInstant<T>::getTimestamp)
      .def("getValue", &TInstant<T>::getValue);

  def_comparator<TemporalComparators<TInstantSet<T>>>(m, "TInstantSet",
                                                      typesuffix);
  def_tinstant_functions<TInstantFunctions<TInstantSet<T>, TInstant<T>, T>>(
      m, "TInstantSet", typesuffix);
  py::class_<TInstantSet<T>, TemporalComparators<TInstantSet<T>>,
             TInstantFunctions<TInstantSet<T>, TInstant<T>, T>>(
      m, ("TInstantSet" + typesuffix).c_str())
      .def(py::init<set<TInstant<T>> &>())
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def(py::self < py::self)
      .def(py::self <= py::self)
      .def(py::self > py::self)
      .def(py::self >= py::self)
      .def("__str__",
           [](TInstantSet<T> const &self) {
             std::ostringstream s;
             s << self;
             return s.str();
           })
      .def("getInstants", &TInstantSet<T>::getInstants);

  def_comparator<TemporalComparators<TSequence<T>>>(m, "TSequence", typesuffix);
  def_tinstant_functions<TInstantFunctions<TSequence<T>, TInstant<T>, T>>(
      m, "TSequence", typesuffix);
  py::class_<TSequence<T>, TemporalComparators<TSequence<T>>,
             TInstantFunctions<TSequence<T>, TInstant<T>, T>>(
      m, ("TSequence" + typesuffix).c_str())
      .def(py::init<vector<TInstant<T>> &, bool, bool>())
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def(py::self < py::self)
      .def(py::self <= py::self)
      .def(py::self > py::self)
      .def(py::self >= py::self)
      .def("__str__",
           [](TSequence<T> const &self) {
             std::ostringstream s;
             s << self;
             return s.str();
           })
      .def_readonly("lower_inc", &TSequence<T>::lower_inc)
      .def_readonly("upper_inc", &TSequence<T>::upper_inc)
      .def("getInstants", &TSequence<T>::getInstants);
}

void def_temporal_module(py::module &m) {
  py::module temporal_module = m.def_submodule(
      "temporal", "This module defines MobilityDB's temporal types: Temporal, "
                  "TInstant, TInstantSet, TSequence and TSequenceSet");

  declare_temporal_types<bool>(temporal_module, "Bool");
  declare_temporal_types<int>(temporal_module, "Int");
  declare_temporal_types<float>(temporal_module, "Float");
  declare_temporal_types<string>(temporal_module, "Text");
  declare_temporal_types<Geometry>(temporal_module, "Geom");
}
