#include <meos/io/Deserializer.hpp>
#include <meos/io/Serializer.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantSet.hpp>
#include <pybind11/chrono.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
extern "C" {
#include <meos/geos.h>
}

using namespace pybind11::literals;
namespace py = pybind11;

py::object time_t_to_datetime(time_t tt) {
  py::module datetime_module = py::module::import("datetime");

  auto tzinfo = datetime_module.attr("timezone").attr("utc");
  return datetime_module.attr("datetime")
      .attr("fromtimestamp")(tt / 1000, "tz"_a = tzinfo);
}

time_t datetime_to_time_t(py::object dt) {
  double seconds = dt.attr("timestamp")().cast<py::float_>();
  return seconds * 1000;
}

py::object time_t_to_timedelta(time_t tt) {
  py::module datetime_module = py::module::import("datetime");

  auto tzinfo = datetime_module.attr("timezone").attr("utc");
  return datetime_module.attr("timedelta")("milliseconds"_a = tt);
}

time_t timedelta_to_time_t(py::object delta_ms) {
  py::object total_seconds_object = delta_ms.attr("total_seconds")();
  float total_seconds = total_seconds_object.cast<py::float_>();
  return total_seconds * 1000;
}

template <typename T>
void declare_temporal_types(py::module &m, std::string const &typesuffix) {
  py::class_<Serializer<T>>(m, ("Serializer" + typesuffix).c_str())
      .def(py::init<>())
      .def("write", (string(Serializer<T>::*)(TInstant<T> const *)) &
                        Serializer<T>::write)
      .def("write", (string(Serializer<T>::*)(TInstantSet<T> const *)) &
                        Serializer<T>::write)
      .def("write", (string(Serializer<T>::*)(TSequence<T> const *)) &
                        Serializer<T>::write)
      .def("write",
           (string(Serializer<T>::*)(Period const *)) & Serializer<T>::write)
      .def("write", (string(Serializer<T>::*)(PeriodSet const *)) &
                        Serializer<T>::write);

  py::class_<Deserializer<T>>(m, ("Deserializer" + typesuffix).c_str())
      .def(py::init<string const &>())
      .def("nextTInstant", &Deserializer<T>::nextTInstant)
      .def("nextTInstantSet", &Deserializer<T>::nextTInstantSet)
      .def("nextTSequence", &Deserializer<T>::nextTSequence)
      .def("nextPeriod", &Deserializer<T>::nextPeriod)
      .def("nextPeriodSet", &Deserializer<T>::nextPeriodSet);

  py::class_<TInstant<T>>(m, ("TInstant" + typesuffix).c_str())
      .def(py::init<T, time_t>())
      .def(
          "__eq__",
          [](TInstant<T> const &self, TInstant<T> const &other) {
            return self == other;
          },
          py::is_operator())
      .def("__hash__",
           [](TInstant<T> const &instant) {
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
      .def(py::init<time_t const, time_t const, bool const, bool const>(),
           py::arg("lower"), py::arg("upper"), py::arg("lower_inc") = true,
           py::arg("upper_inc") = false)
      .def(py::init<string const, string const, bool const, bool const>(),
           py::arg("lower"), py::arg("upper"), py::arg("lower_inc") = true,
           py::arg("upper_inc") = false)
      .def(py::init<string const>(), py::arg("serialized"))
      .def(py::init([](py::object lower_dt, py::object upper_dt,
                       bool const lower_inc, bool const upper_inc) {
             time_t lower = datetime_to_time_t(lower_dt);
             time_t upper = datetime_to_time_t(upper_dt);
             return Period(lower, upper, lower_inc, upper_inc);
           }),
           py::arg("lower"), py::arg("upper"), py::arg("lower_inc") = true,
           py::arg("upper_inc") = false)
      .def(py::self == py::self)
      .def(py::self < py::self)
      .def(py::self <= py::self)
      .def(py::self > py::self)
      .def(py::self >= py::self)
      .def("__hash__",
           [](Period const &period) {
             return py::hash(py::make_tuple(period.lower(), period.upper(),
                                            period.lower_inc(),
                                            period.upper_inc()));
           })
      .def("__str__",
           [](Period const &self) {
             std::ostringstream s;
             s << self;
             return s.str();
           })
      .def_property_readonly("lower",
                             [](Period const &period) {
                               return time_t_to_datetime(period.lower());
                             })
      .def_property_readonly("upper",
                             [](Period const &period) {
                               return time_t_to_datetime(period.upper());
                             })
      .def_property_readonly("lower_inc", &Period::lower_inc)
      .def_property_readonly("upper_inc", &Period::upper_inc)
      .def_property_readonly("timespan",
                             [](Period const &period) {
                               return time_t_to_timedelta(period.timespan());
                             })
      .def("shift",
           [](Period const &period, py::object delta_ms) {
             return period.shift(timedelta_to_time_t(delta_ms));
           })
      .def("overlap", &Period::overlap)
      .def("contains_timestamp", [](Period const &period, py::object dt) {
        return period.contains_timestamp(datetime_to_time_t(dt));
      });

  py::class_<PeriodSet>(m, "PeriodSet")
      .def(py::init<set<Period> &>())
      .def("periods", &PeriodSet::periods);
}
