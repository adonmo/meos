#ifndef MEOS_TYPES_TEMPORAL_TSEQUENCE_HPP
#define MEOS_TYPES_TEMPORAL_TSEQUENCE_HPP

#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <vector>

using namespace std;

template <typename T> class TSequence : public Temporal<T> {
public:
  vector<unique_ptr<TInstant<T>>> instants;
  bool left_open;
  bool right_open;

  TSequence(vector<unique_ptr<TInstant<T>>> &instants_, bool left_open,
            bool right_open);

  TSequence(vector<TInstant<T>> &instants_, bool left_open, bool right_open);

  vector<TInstant<T>> getInstants();

protected:
  TSequence(const TSequence &t);

private:
};

template class TSequence<int>;
template class TSequence<float>;

#endif
