#include <iomanip>
#include <sstream>
#include <string>

#include <meos/types/geom/Geometry.hpp>
#include <meos/util/serializing.hpp>

std::string write_ISO8601_time(time_point const &t) {
  std::stringstream textStream;
  long int tt = std::chrono::time_point_cast<std::chrono::milliseconds>(t)
                    .time_since_epoch()
                    .count();
  int millis = tt % 1000;
  tt = tt / 1000;

  textStream << std::put_time(gmtime(&tt), "%FT%T");

  if (millis > 0) {
    textStream << "." << std::setfill('0') << std::setw(3) << millis;
  }

  textStream << std::put_time(gmtime(&tt), "%z");
  return textStream.str();
}
