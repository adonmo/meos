#ifndef MEOS_TO_PARSEXCEPTION_HPP
#define MEOS_TO_PARSEXCEPTION_HPP

#include <stdexcept>
#include <string>

using namespace std;

class ParseException: public runtime_error {
public:
    ParseException()
        :
        runtime_error("Unknown error")
    {}

    ParseException(string const& msg)
        :
        runtime_error(msg)
    {}

    ParseException(string const& name, string const& msg)
        :
        runtime_error(name + ": " + msg)
    {}
};

#endif
