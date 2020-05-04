#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantSet.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

template <typename T>
void declare_temporal_types(py::module &m, const std::string &typesuffix) {
  py::class_<Serializer<T>>(m, ("Serializer" + typesuffix).c_str())
      .def(py::init<>())
      .def("write",
           (string(Serializer<T>::*)(TInstant<T> *)) & Serializer<T>::write)
      .def("write",
           (string(Serializer<T>::*)(TInstantSet<T> *)) & Serializer<T>::write)
      .def("write",
           (string(Serializer<T>::*)(TSequence<T> *)) & Serializer<T>::write);

  py::class_<Deserializer<T>>(m, ("Deserializer" + typesuffix).c_str())
      .def(py::init<const string &>())
      .def("nextTInstant", &Deserializer<T>::nextTInstant)
      .def("nextTInstantSet", &Deserializer<T>::nextTInstantSet)
      .def("nextTSequence", &Deserializer<T>::nextTSequence);

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

// TODO FIXME
void notice(const char *fmt, ...) {}

void GEOS_initGEOS() { initGEOS(notice, notice); }

PYBIND11_MODULE(pymeos, m) {
  py::class_<Geometry>(m, "Geometry")
      .def(py::init<std::string>())
      .def(py::init<double, double>())
      .def("fromWKT", &Geometry::fromWKT)
      .def("toWKT", &Geometry::toWKT);

  m.def("initGEOS", &GEOS_initGEOS);
  m.def("finishGEOS", &finishGEOS);

  declare_temporal_types<bool>(m, "Bool");
  declare_temporal_types<int>(m, "Int");
  declare_temporal_types<float>(m, "Float");
  declare_temporal_types<string>(m, "Text");
  declare_temporal_types<Geometry>(m, "Geom");
}
