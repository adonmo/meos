#ifndef MEOS_TYPES_TEMPORAL_TSEQUENCE_HPP
#define MEOS_TYPES_TEMPORAL_TSEQUENCE_HPP

#include <meos/types/geom/Geometry.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantFunctions.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/util/time.hpp>
#include <vector>

using namespace std;

template <typename T = float>
class TSequence : public Temporal<T>,
                  public TInstantFunctions<TSequence<T>, TInstant<T>> {
public:
  vector<unique_ptr<TInstant<T>>> m_instants;
  bool lower_inc;
  bool upper_inc;

  TSequence(vector<unique_ptr<TInstant<T>>> &instants_, bool lower_inc,
            bool upper_inc);

  TSequence(vector<TInstant<T>> &instants_, bool lower_inc, bool upper_inc);

  unique_ptr<TSequence<T>> clone() const {
    return std::unique_ptr<TSequence<T>>(this->clone_impl());
  }

  vector<TInstant<T>> getInstants() const;

  string duration() const { return "Sequence"; };

  /**
   * Set of instants.
   */
  set<TInstant<T>> instants() const;

  set<time_t> timestamps() const override;
  const PeriodSet getTime() const;
  const Period period() const override;
  unique_ptr<TSequence<T>> shift(const time_t timedelta) const;
  TSequence<T> *shift_impl(const time_t timedelta) const override;
  bool intersectsTimestamp(const time_t datetime) const override;
  bool intersectsPeriod(const Period period) const override;

  friend ostream &operator<<(ostream &os, const TSequence<T> &sequence) {
    bool first = true;
    os << (sequence.lower_inc ? "[" : "(");
    for (auto const &instant : sequence.getInstants()) {
      if (first)
        first = false;
      else
        os << ", ";
      os << instant;
    }
    os << (sequence.upper_inc ? "]" : ")");
    return os;
  }

protected:
  TSequence(const TSequence &t);

private:
  TSequence<T> *clone_impl() const override { return new TSequence<T>(*this); };
};

template class TSequence<bool>;
template class TSequence<int>;
template class TSequence<float>;
template class TSequence<string>;
template class TSequence<Geometry>;

#endif
