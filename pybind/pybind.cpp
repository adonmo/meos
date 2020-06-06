#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantSet.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
extern "C" {
#include <meos/geos.h>
}

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
      .def(
          "__eq__",
          [](TInstant<T> const &self, TInstant<T> const &other) {
            return self == other;
          },
          py::is_operator())
      .def("__hash__",
           [](const TInstant<T> &instant) {
             return py::hash(
                 py::make_tuple(instant.getValue(), instant.getTimestamp()));
           })
      .def("getTimestamp", &TInstant<T>::getTimestamp)
      .def("getValue", &TInstant<T>::getValue);

  py::class_<TInstantSet<T>>(m, ("TInstantSet" + typesuffix).c_str())
      .def(py::init<set<TInstant<T>> &>())
      .def("getInstants", &TInstantSet<T>::getInstants);

  py::class_<TSequence<T>>(m, ("TSequence" + typesuffix).c_str())
      .def(py::init<vector<TInstant<T>> &, bool, bool>())
      .def_readonly("lower_inc", &TSequence<T>::lower_inc)
      .def_readonly("upper_inc", &TSequence<T>::upper_inc)
      .def("getInstants", &TSequence<T>::getInstants);
}

PYBIND11_MODULE(pymeos, m) {
  // TODO will we ever get a chance to do finish_geos()?
  init_geos();
  py::class_<Geometry>(m, "Geometry")
      .def(py::init<std::string>())
      .def(py::init<double, double>())
      .def("fromWKT", &Geometry::fromWKT)
      .def("toWKT", &Geometry::toWKT);

  declare_temporal_types<bool>(m, "Bool");
  declare_temporal_types<int>(m, "Int");
  declare_temporal_types<float>(m, "Float");
  declare_temporal_types<string>(m, "Text");
  declare_temporal_types<Geometry>(m, "Geom");

  py::class_<Period>(m, "Period")
      .def(py::init<time_t, time_t, const bool, const bool>())
      .def(
          "__eq__",
          [](Period const &self, Period const &other) { return self == other; },
          py::is_operator())
      .def("__hash__",
           [](const Period &period) {
             return py::hash(py::make_tuple(period.lower(), period.upper(),
                                            period.lower_inc(),
                                            period.upper_inc()));
           })
      .def("lower", &Period::lower)
      .def("upper", &Period::upper)
      .def("lower_inc", &Period::lower_inc)
      .def("upper_inc", &Period::upper_inc);

  py::class_<PeriodSet>(m, "PeriodSet")
      .def(py::init<set<Period> &>())
      .def("periods", &PeriodSet::periods);
}
