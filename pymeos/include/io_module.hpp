#pragma once

#include <pybind11/pybind11.h>

#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>
#include <string>

using namespace meos;
namespace py = pybind11;

template <typename T> void declare_serdes(py::module &m, std::string const &typesuffix) {
  py::class_<Serializer<T>>(m, ("Serializer" + typesuffix).c_str())
      .def(py::init<>())
      .def("write", (std::string(Serializer<T>::*)(TInstant<T> const *)) & Serializer<T>::write)
      .def("write", (std::string(Serializer<T>::*)(TInstantSet<T> const *)) & Serializer<T>::write)
      .def("write", (std::string(Serializer<T>::*)(TSequence<T> const *)) & Serializer<T>::write)
      .def("write", (std::string(Serializer<T>::*)(TSequenceSet<T> const *)) & Serializer<T>::write)
      .def("write", (std::string(Serializer<T>::*)(Period const *)) & Serializer<T>::write)
      .def("write", (std::string(Serializer<T>::*)(PeriodSet const *)) & Serializer<T>::write);

  py::class_<Deserializer<T>>(m, ("Deserializer" + typesuffix).c_str())
      .def(py::init<std::string const &>())
      .def("nextTemporal", &Deserializer<T>::nextTemporal)
      .def("nextTInstant", &Deserializer<T>::nextTInstant)
      .def("nextTInstantSet", &Deserializer<T>::nextTInstantSet)
      .def("nextTSequence", &Deserializer<T>::nextTSequence)
      .def("nextTSequenceSet", &Deserializer<T>::nextTSequenceSet)
      .def("nextPeriod", &Deserializer<T>::nextPeriod)
      .def("nextPeriodSet", &Deserializer<T>::nextPeriodSet);
}

void def_io_module(py::module &m) {
  py::module io_module = m.def_submodule("io",
                                         "This module defines serializer and desializers "
                                         "for MobilityDB data types");

  declare_serdes<bool>(io_module, "Bool");
  declare_serdes<int>(io_module, "Int");
  declare_serdes<float>(io_module, "Float");
  declare_serdes<std::string>(io_module, "Text");
  declare_serdes<GeomPoint>(io_module, "Geom");
}
