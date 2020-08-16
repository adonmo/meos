#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <meos/geos.hpp>
#include <sstream>
#include <string>

#include "box_module.hpp"
#include "common.hpp"
#include "geometry.hpp"
#include "io_module.hpp"
#include "range_module.hpp"
#include "temporal_module.hpp"
#include "time_module.hpp"

using namespace meos;
namespace py = pybind11;

PYBIND11_MODULE(_pymeos, m) {
  // TODO will we ever get a chance to do finish_geos()?
  init_geos();

  def_box_module(m);
  def_io_module(m);
  def_range_module(m);
  def_temporal_module(m);
  def_time_module(m);
}
