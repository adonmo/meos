#pragma once

#include <pybind11/pybind11.h>

#include <meos/types/temporal/Interpolation.hpp>
#include <meos/types/temporal/TemporalDuration.hpp>
#include <string>

#include "common.hpp"
#include "temporal/temporal.hpp"
#include "temporal/tinstant.hpp"
#include "temporal/tinstantset.hpp"
#include "temporal/tsequence.hpp"
#include "temporal/tsequenceset.hpp"

namespace py = pybind11;

template <typename BaseType> void def_temporal_types(py::module &m, std::string const &typesuffix) {
  def_temporal_class<BaseType>(m, typesuffix);
  def_tinstant_class<BaseType>(m, typesuffix);
  def_tinstantset_class<BaseType>(m, typesuffix);
  def_tsequence_class<BaseType>(m, typesuffix);
  def_tsequenceset_class<BaseType>(m, typesuffix);
}

void def_temporal_module(py::module &m) {
  py::module temporal_module
      = m.def_submodule("temporal",
                        "This module defines MobilityDB's temporal types: Temporal, "
                        "TInstant, TInstantSet, TSequence and TSequenceSet");

  py::enum_<TemporalDuration>(temporal_module, "TemporalDuration")
      .value("Temporal", TemporalDuration::Temporal)
      .value("Instant", TemporalDuration::Instant)
      .value("InstantSet", TemporalDuration::InstantSet)
      .value("Sequence", TemporalDuration::Sequence)
      .value("SequenceSet", TemporalDuration::SequenceSet);

  py::enum_<Interpolation>(temporal_module, "Interpolation")
      .value("Stepwise", Interpolation::Stepwise)
      .value("Linear", Interpolation::Linear);

  py::class_<Empty>(m, "Empty");

  py::class_<SRIDMembers>(m, "SRIDMembers").def_property_readonly("srid", &SRIDMembers::srid);

  def_temporal_types<bool>(temporal_module, "Bool");
  def_temporal_types<int>(temporal_module, "Int");
  def_temporal_types<float>(temporal_module, "Float");
  def_temporal_types<string>(temporal_module, "Text");
  def_temporal_types<Geometry>(temporal_module, "Geom");
}
