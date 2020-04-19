#include <meos/io/Parser.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantSet.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(libmeos, m) {
  // TODO instead of hardcoding int, make it generic
  py::class_<Parser<int>>(m, "Parser")
      .def(py::init<const string &>())
      .def("parseNextTInstant", &Parser<int>::parseNextTInstant)
      .def("parseNextTInstantSet", &Parser<int>::parseNextTInstantSet)
      .def("parseNextTSequence", &Parser<int>::parseNextTSequence);

  py::class_<TInstant<int>>(m, "TInstant")
      .def(py::init<int, time_t>())
      .def("getT", &TInstant<int>::getT)
      .def("getValue", &TInstant<int>::getValue);

  py::class_<TInstantSet<int>>(m, "TInstantSet")
      .def(py::init<set<TInstant<int>> &>())
      .def("getInstants", &TInstantSet<int>::getInstants);

  py::class_<TSequence<int>>(m, "TSequence")
      .def(py::init<vector<TInstant<int>> &, bool, bool>())
      .def_readonly("left_open", &TSequence<int>::left_open)
      .def_readonly("right_open", &TSequence<int>::right_open)
      .def("getInstants", &TSequence<int>::getInstants);
}
