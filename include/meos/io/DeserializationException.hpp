#ifndef MEOS_IO_DESERIALIZATIONEXCEPTION_HPP
#define MEOS_IO_DESERIALIZATIONEXCEPTION_HPP

#include <stdexcept>
#include <string>

using namespace std;

class DeserializationException : public runtime_error {
public:
  DeserializationException() : runtime_error("Unknown error") {}

  DeserializationException(string const &msg) : runtime_error(msg) {}

  DeserializationException(string const &name, string const &msg)
      : runtime_error(name + ": " + msg) {}
};

#endif
