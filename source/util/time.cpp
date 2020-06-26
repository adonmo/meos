#include <meos/util/time.hpp>

std::string ISO8601_time(time_t const &t) {
  std::stringstream textStream;
  // TODO: FIXME as of now, milliseconds are lost
  time_t tt(t);
  tt = tt / 1000;
  textStream << std::put_time(gmtime(&tt), "%FT%T%z");
  return textStream.str();
};
