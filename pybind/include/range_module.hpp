#pragma once

#include "chrono.h"
#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantSet.hpp>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

template <typename T>
void def_range_type(py::module &m, std::string const &typesuffix) {
  py::class_<Range<T>>(m, ("Range" + typesuffix).c_str())
      .def(py::init<T, T, bool, bool>(), py::arg("lower"), py::arg("upper"),
           py::arg("lower_inc") = true, py::arg("upper_inc") = false)
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def(py::self < py::self)
      .def(py::self <= py::self)
      .def(py::self > py::self)
      .def(py::self >= py::self)
      .def("__hash__",
           [](Range<T> const &range) {
             return py::hash(py::make_tuple(range.lower(), range.upper(),
                                            range.lower_inc(),
                                            range.upper_inc()));
           })
      .def("__str__",
           [](Range<T> const &self) {
             std::ostringstream s;
             s << self;
             return s.str();
           })
      .def_property_readonly("lower", &Range<T>::lower)
      .def_property_readonly("upper", &Range<T>::upper)
      .def_property_readonly("lower_inc", &Range<T>::lower_inc)
      .def_property_readonly("upper_inc", &Range<T>::upper_inc)
      .def("compare", &Range<T>::compare)
      .def("shift", &Range<T>::shift)
      .def("overlap", &Range<T>::overlap)
      .def("contains", &Range<T>::contains);
}

void def_range_module(py::module &m) {
  py::module range_module =
      m.def_submodule("range", "This module defines Range type similar to the "
                               "one used in Postgres and MobilityDB");

  def_range_type<bool>(range_module, "Bool");
  def_range_type<int>(range_module, "Int");
  def_range_type<float>(range_module, "Float");
  def_range_type<string>(range_module, "Text");
  def_range_type<Geometry>(range_module, "Geom");
}
