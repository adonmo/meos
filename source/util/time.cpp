#include <iomanip>
#include <meos/util/time.hpp>
#include <sstream>

std::string ISO8601_time(time_point const &t) {
  std::stringstream textStream;
  long int tt = std::chrono::time_point_cast<std::chrono::milliseconds>(t)
                    .time_since_epoch()
                    .count();
  // TODO: FIXME as of now, milliseconds are dropped
  tt = tt / 1000;
  textStream << std::put_time(gmtime(&tt), "%FT%T%z");
  return textStream.str();
};
