#pragma once

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <meos/types/temporal/Interpolation.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TSequence.hpp>
#include <meos/types/temporal/TemporalSet.hpp>
#include <sstream>
#include <string>

#include "temporalset.hpp"

namespace py = pybind11;

template <typename BaseType> using py_tsequence
    = py::class_<TSequence<BaseType>, TemporalSet<BaseType>>;

template <typename BaseType>
py_tsequence<BaseType> _def_tsequence_class_basic(py::module &m,
                                                  std::string const &base_type_name) {
  return py_tsequence<BaseType>(m, ("T" + base_type_name + "Seq").c_str())
      .def(py::init<set<TInstant<BaseType>> &, bool, bool, Interpolation>(), py::arg("instants"),
           py::arg("lower_inc") = true, py::arg("upper_inc") = false,
           py::arg("interpolation") = default_interp_v<BaseType>)
      .def(py::init<set<string> &, bool, bool, Interpolation>(), py::arg("instants"),
           py::arg("lower_inc") = true, py::arg("upper_inc") = false,
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
      .def_property_readonly("interpolation", &TSequence<BaseType>::interpolation)
      .def_property_readonly("timespan", &TSequence<BaseType>::timespan)
      .def_property_readonly("getValues", &TSequence<BaseType>::getValues)
      .def_property_readonly("getTime", &TSequence<BaseType>::getTime)
      .def_property_readonly("period", &TSequence<BaseType>::period)
      .def("shift", &TSequence<BaseType>::shift, py::arg("timedelta"))
      .def("intersectsTimestamp", &TSequence<BaseType>::intersectsTimestamp, py::arg("datetime"))
      .def("intersectsPeriod", &TSequence<BaseType>::intersectsPeriod, py::arg("period"));
}

template <typename BaseType>
void _def_tsequence_class_specializations(py_tsequence<BaseType> &c,
                                          std::string const &base_type_name) {
  // No specializations by default
}

template <> void _def_tsequence_class_specializations(py_tsequence<GeomPoint> &c,
                                                      std::string const &base_type_name) {
  c.def(py::init<set<TInstant<GeomPoint>> &, bool, bool, int, Interpolation>(), py::arg("instants"),
        py::arg("lower_inc") = true, py::arg("upper_inc") = false, py::arg("srid") = 0,
        py::arg("interpolation") = default_interp_v<GeomPoint>)
      .def(py::init<set<string> &, bool, bool, int, Interpolation>(), py::arg("instants"),
           py::arg("lower_inc") = true, py::arg("upper_inc") = false, py::arg("srid") = 0,
           py::arg("interpolation") = default_interp_v<GeomPoint>)
      .def(py::init<string, int>(), py::arg("serialized"), py::arg("srid"));
}

template <typename BaseType>
void def_tsequence_class(py::module &m, std::string const &base_type_name) {
  auto tsequence_class = _def_tsequence_class_basic<BaseType>(m, base_type_name);
  _def_tsequence_class_specializations<BaseType>(tsequence_class, base_type_name);
}
