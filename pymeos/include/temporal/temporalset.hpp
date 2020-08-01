#pragma once

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TemporalSet.hpp>
#include <string>

#include "common.hpp"

using namespace meos;
namespace py = pybind11;

template <typename BaseType> using py_temporalset
    = py::class_<TemporalSet<BaseType>, Temporal<BaseType>,
                 TemporalComparators<TemporalSet<BaseType>>,
                 TInstantFunctions<TemporalSet<BaseType>, TInstant<BaseType>, BaseType>>;

template <typename BaseType>
void def_temporalset_class(py::module &m, std::string const &base_type_name) {
  def_comparator<TemporalComparators<TemporalSet<BaseType>>>(m, "Set", base_type_name);
  def_tinstant_functions<TInstantFunctions<TemporalSet<BaseType>, TInstant<BaseType>, BaseType>>(
      m, "Set", base_type_name);
  py_temporalset<BaseType>(m, ("T" + base_type_name + "Set").c_str())
      .def_property_readonly("instants", &TemporalSet<BaseType>::instants)
      .def_property_readonly("timestamps", &TemporalSet<BaseType>::timestamps);
}
