#pragma once

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TemporalSet.hpp>
#include <string>

#include "common.hpp"

namespace py = pybind11;

template <typename BaseType> using py_temporalset
    = py::class_<TemporalSet<BaseType>, Temporal<BaseType>,
                 TemporalComparators<TemporalSet<BaseType>>,
                 TInstantFunctions<TemporalSet<BaseType>, TInstant<BaseType>, BaseType>>;

template <typename BaseType>
void def_temporalset_class(py::module &m, std::string const &typesuffix) {
  def_comparator<TemporalComparators<TemporalSet<BaseType>>>(m, "TemporalSet", typesuffix);
  def_tinstant_functions<TInstantFunctions<TemporalSet<BaseType>, TInstant<BaseType>, BaseType>>(
      m, "TemporalSet", typesuffix);
  py_temporalset<BaseType>(m, ("TemporalSet" + typesuffix).c_str())
      .def_property_readonly("instants", &TemporalSet<BaseType>::instants)
      .def_property_readonly("timestamps", &TemporalSet<BaseType>::timestamps);
}
