#include "io_module.hpp"
#include "temporal_module.hpp"
#include "time_module.hpp"
#include <pybind11/pybind11.h>
extern "C" {
#include <meos/geos.h>
}

namespace py = pybind11;

PYBIND11_MODULE(pymeos, m) {
  // TODO will we ever get a chance to do finish_geos()?
  init_geos();
  py::class_<Geometry>(m, "Geometry")
      .def(py::init<std::string>())
      .def(py::init<double, double>())
      .def("fromWKT", &Geometry::fromWKT)
      .def("toWKT", &Geometry::toWKT);

  def_io_module(m);
  def_temporal_module(m);
  def_time_module(m);
}
