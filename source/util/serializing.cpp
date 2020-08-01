#include <iomanip>
#include <meos/types/geom/GeomPoint.hpp>
#include <meos/util/serializing.hpp>
#include <sstream>
#include <string>

std::string write_ISO8601_time(time_point const &t) {
  std::stringstream textStream;
  time_t tt = std::chrono::time_point_cast<std::chrono::milliseconds>(t).time_since_epoch().count();
  int millis = tt % 1000;
  tt = tt / 1000;

  textStream << std::put_time(gmtime(&tt), "%FT%T");

  if (millis > 0) {
    textStream << "." << std::setfill('0') << std::setw(3) << millis;
  }

  // We only output time in UTC
  textStream << "+0000";
  return textStream.str();
}
