#pragma once

#include <pybind11/pybind11.h>

#include <string>

namespace py = pybind11;

template <class Interface> void def_comparator(const pybind11::module &m,
                                               std::string const &temporal_duration_name,
                                               std::string const &base_type_name) {
  py::class_<Interface>(m,
                        ("TemporalComparatorsT" + base_type_name + temporal_duration_name).c_str());
}

template <class Interface> void def_tinstant_functions(const pybind11::module &m,
                                                       std::string const &temporal_duration_name,
                                                       std::string const &base_type_name) {
  py::class_<Interface>(m, ("TInstantFunctionsT" + base_type_name + temporal_duration_name).c_str())
      .def_property_readonly("numInstants", &Interface::numInstants)
      .def_property_readonly("startInstant", &Interface::startInstant)
      .def_property_readonly("endInstant", &Interface::endInstant)
      .def("instantN", &Interface::instantN, py::arg("n"))
      .def_property_readonly("startValue", &Interface::startValue)
      .def_property_readonly("endValue", &Interface::endValue)
      .def("valueN", &Interface::valueN, py::arg("n"));
}