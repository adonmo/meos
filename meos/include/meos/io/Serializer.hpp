#ifndef MEOS_IO_SERIALIZER_HPP
#define MEOS_IO_SERIALIZER_HPP

#include <iomanip>
#include <meos/types/geom/Geometry.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantSet.hpp>
#include <meos/types/temporal/TSequence.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <string>

using namespace std;

template <typename T> class Serializer {
public:
  string write(Temporal<T> *temporal);
  string write(TInstant<T> *instant);
  string write(TInstantSet<T> *instant_set);
  string write(TSequence<T> *sequence);

  string write(const T &value);

  /**
   * Serialize time in ISO8601 format
   */
  string writeTime(const time_t &time);

private:
};

template class Serializer<bool>;
template class Serializer<int>;
template class Serializer<float>;
template class Serializer<string>;
template class Serializer<Geometry>;

#endif
