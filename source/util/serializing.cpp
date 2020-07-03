#include <iomanip>
#include <meos/types/geom/Geometry.hpp>
#include <meos/util/serializing.hpp>
#include <sstream>

std::string write_ISO8601_time(time_point const &t) {
  std::stringstream textStream;
  long int tt = std::chrono::time_point_cast<std::chrono::milliseconds>(t)
                    .time_since_epoch()
                    .count();
  // TODO: FIXME as of now, milliseconds are dropped
  tt = tt / 1000;
  textStream << std::put_time(gmtime(&tt), "%FT%T%z");
  return textStream.str();
}

template <typename T> std::string write_value(T const &value) {
  // Check specialized template functions below for supported types
  throw std::invalid_argument("Unsupported type");
}

template <> std::string write_value(bool const &value) {
  return value ? "t" : "f";
}

template <> std::string write_value(int const &value) {
  return std::to_string(value);
}

template <> std::string write_value(float const &value) {
  std::stringstream ss;
  ss << value;
  return ss.str();
}

template <> std::string write_value(std::string const &value) {
  return '"' + value + '"';
}

template <> std::string write_value(Geometry const &value) {
  return value.toWKT();
}
