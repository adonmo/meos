#ifndef MEOS_IO_SERIALIZER_HPP
#define MEOS_IO_SERIALIZER_HPP

#include <iomanip>
#include <meos/types/temporal/Temporal.hpp>
#include <string>

using namespace std;

template <typename T> class Serializer {
public:
  string write(unique_ptr<Temporal<T>> &temporal);

  /**
   * Serialize time in ISO8601 format
   */
  string write(const T &value);
  string writeTime(const time_t &time);

private:
};

template class Serializer<int>;
template class Serializer<float>;

#endif
