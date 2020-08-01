#pragma once

#include <stdexcept>
#include <string>

namespace meos {

class DeserializationException : public std::runtime_error {
public:
  DeserializationException() : std::runtime_error("Unknown error") {}

  DeserializationException(std::string const &msg) : std::runtime_error(msg) {}

  DeserializationException(std::string const &name, std::string const &msg)
      : std::runtime_error(name + ": " + msg) {}
};

}  // namespace meos
