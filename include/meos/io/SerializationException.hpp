#pragma once

#include <stdexcept>
#include <string>

namespace meos {

class SerializationException : public std::runtime_error {
public:
  SerializationException() : std::runtime_error("Unknown error") {}

  SerializationException(std::string const &msg) : std::runtime_error(msg) {}

  SerializationException(std::string const &name, std::string const &msg)
      : std::runtime_error(name + ": " + msg) {}
};

}  // namespace meos
