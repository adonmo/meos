#ifndef MEOS_TYPES_TEMPORAL_TSEQUENCE_HPP
#define MEOS_TYPES_TEMPORAL_TSEQUENCE_HPP

#include <meos/io/utils.hpp>
#include <meos/types/geom/Geometry.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantFunctions.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/types/temporal/TemporalComparators.hpp>
#include <meos/util/serializing.hpp>
#include <vector>

using namespace std;

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

template <typename T = float>
class TSequence : public Temporal<T>,
                  public TemporalComparators<TSequence<T>>,
                  public TInstantFunctions<TSequence<T>, TInstant<T>, T> {
public:
  vector<unique_ptr<TInstant<T>>> m_instants;

  TSequence();
  TSequence(vector<unique_ptr<TInstant<T>>> &instants_, bool lower_inc = true,
            bool upper_inc = false);
  TSequence(vector<TInstant<T>> &instants_, bool lower_inc = true,
            bool upper_inc = false);
  TSequence(vector<string> const &instants, bool lower_inc = true,
            bool upper_inc = false);
  TSequence(string const &serialized);

  int compare(Temporal<T> const &other) const override;

  unique_ptr<TSequence<T>> clone() const {
    return std::unique_ptr<TSequence<T>>(this->clone_impl());
  }

  bool lower_inc() const;
  bool upper_inc() const;
  vector<TInstant<T>> getInstants() const;

  TemporalDuration const duration() const {
    return TemporalDuration::Sequence;
  };

  /**
   * Set of instants.
   */
  set<TInstant<T>> instants() const;

  set<Range<T>> getValues() const override;
  set<time_point> timestamps() const override;
  PeriodSet getTime() const;
  Period period() const override;
  unique_ptr<TSequence<T>> shift(duration_ms const timedelta) const;
  TSequence<T> *shift_impl(duration_ms const timedelta) const override;
  bool intersectsTimestamp(time_point const datetime) const override;
  bool intersectsPeriod(Period const period) const override;

  friend istream &operator>>(istream &in, TSequence &sequence) {
    char c;

    c = consume_one_of(in, "[(");
    bool const lower_inc = c == '[';

    vector<unique_ptr<TInstant<T>>> s = {};

    TInstant<T> instant;
    in >> instant;
    s.push_back(instant.clone());

    while (true) {
      in >> c;
      if (c != ',')
        break;
      in >> instant;
      s.push_back(instant.clone());
    }

    if (c != ']' && c != ')') {
      throw invalid_argument("Expected either a ']' or ')'");
    }
    bool const upper_inc = c == ']';

    sequence.m_instants.empty();
    for (auto const &e : s)
      sequence.m_instants.push_back(e->clone());

    sequence.m_lower_inc = lower_inc;
    sequence.m_upper_inc = upper_inc;

    return in;
  }

  friend ostream &operator<<(ostream &os, TSequence const &sequence) {
    bool first = true;
    os << (sequence.m_lower_inc ? "[" : "(");
    for (auto const &instant : sequence.getInstants()) {
      if (first)
        first = false;
      else
        os << ", ";
      os << instant;
    }
    os << (sequence.m_upper_inc ? "]" : ")");
    return os;
  }

protected:
  TSequence(TSequence const &t);

private:
  bool m_lower_inc;
  bool m_upper_inc;

  TSequence<T> *clone_impl() const override { return new TSequence<T>(*this); };
};

template class TSequence<bool>;
template class TSequence<int>;
template class TSequence<float>;
template class TSequence<string>;
template class TSequence<Geometry>;

#endif
