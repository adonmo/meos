#ifndef MEOS_IO_SERIALIZATIONEXCEPTION_HPP
#define MEOS_IO_SERIALIZATIONEXCEPTION_HPP

#include <stdexcept>
#include <string>

using namespace std;

class SerializationException : public runtime_error {
public:
  SerializationException() : runtime_error("Unknown error") {}

  SerializationException(string const &msg) : runtime_error(msg) {}

  SerializationException(string const &name, string const &msg)
      : runtime_error(name + ": " + msg) {}
};

#endif
