#include <meos/io/Parser.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantSet.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

template <typename T>
void declare_temporal_types(py::module &m, const std::string &typesuffix) {
  py::class_<Parser<T>>(m, ("Parser" + typesuffix).c_str())
      .def(py::init<const string &>())
      .def("parseNextTInstant", &Parser<T>::parseNextTInstant)
      .def("parseNextTInstantSet", &Parser<T>::parseNextTInstantSet)
      .def("parseNextTSequence", &Parser<T>::parseNextTSequence);

  py::class_<TInstant<T>>(m, ("TInstant" + typesuffix).c_str())
      .def(py::init<T, time_t>())
      .def("getT", &TInstant<T>::getT)
      .def("getValue", &TInstant<T>::getValue);

  py::class_<TInstantSet<T>>(m, ("TInstantSet" + typesuffix).c_str())
      .def(py::init<set<TInstant<T>> &>())
      .def("getInstants", &TInstantSet<T>::getInstants);

  py::class_<TSequence<T>>(m, ("TSequence" + typesuffix).c_str())
      .def(py::init<vector<TInstant<T>> &, bool, bool>())
      .def_readonly("left_open", &TSequence<T>::left_open)
      .def_readonly("right_open", &TSequence<T>::right_open)
      .def("getInstants", &TSequence<T>::getInstants);
}

PYBIND11_MODULE(libmeos, m) {
  declare_temporal_types<int>(m, "Int");
  declare_temporal_types<float>(m, "Float");
}
