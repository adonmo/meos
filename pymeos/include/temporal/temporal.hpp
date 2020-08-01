#pragma once

#include <pybind11/pybind11.h>

#include <meos/types/temporal/Temporal.hpp>
#include <string>
#include <type_traits>

using namespace meos;
namespace py = pybind11;

template <typename BaseType> using py_temporal
    = py::class_<Temporal<BaseType>,
                 std::conditional_t<std::is_same<BaseType, GeomPoint>::value, SRIDMembers, Empty>>;

template <typename BaseType> void def_temporal_class(py::module &m, std::string const &typesuffix) {
  py_temporal<BaseType>(m, ("T" + typesuffix).c_str())
      .def_property_readonly("minValue", &Temporal<BaseType>::minValue)
      .def_property_readonly("maxValue", &Temporal<BaseType>::maxValue)
      .def_property_readonly("numTimestamps", &Temporal<BaseType>::numTimestamps)
      .def_property_readonly("startTimestamp", &Temporal<BaseType>::startTimestamp)
      .def_property_readonly("endTimestamp", &Temporal<BaseType>::endTimestamp)
      .def("timestampN", &Temporal<BaseType>::timestampN, py::arg("n"))
      .def("intersectsTimestampSet", &Temporal<BaseType>::intersectsTimestampSet,
           py::arg("timestampset"))
      .def("intersectsPeriodSet", &Temporal<BaseType>::intersectsPeriodSet, py::arg("periodset"));
}
