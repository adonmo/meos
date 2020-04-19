#include <iomanip>
#include <meos/types/temporal/TInstant.hpp>

template <typename T>
TInstant<T>::TInstant(T value_, std::time_t t_) : value(value_), t(t_) {}

template <typename T> T TInstant<T>::getValue() { return this->value; }

template <typename T> std::time_t &TInstant<T>::getT() { return this->t; }
