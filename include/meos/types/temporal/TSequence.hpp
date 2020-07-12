#ifndef MEOS_TYPES_TEMPORAL_TSEQUENCE_HPP
#define MEOS_TYPES_TEMPORAL_TSEQUENCE_HPP

#include <meos/io/utils.hpp>
#include <meos/types/geom/Geometry.hpp>
#include <meos/types/temporal/Interpolation.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantFunctions.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/types/temporal/TemporalComparators.hpp>
#include <meos/util/serializing.hpp>
#include <set>

using namespace std;

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

template <typename T = float>
class TSequence : public Temporal<T>,
                  public TemporalComparators<TSequence<T>>,
                  public TInstantFunctions<TSequence<T>, TInstant<T>, T> {
public:
  TSequence();
  TSequence(set<TInstant<T>> &instants_, bool lower_inc = true,
            bool upper_inc = false,
            Interpolation interpolation = default_interp_v<T>);
  TSequence(set<string> const &instants, bool lower_inc = true,
            bool upper_inc = false,
            Interpolation interpolation = default_interp_v<T>);
  TSequence(string const &serialized);

  int compare(Temporal<T> const &other) const override;

  unique_ptr<TSequence<T>> clone() const {
    return std::unique_ptr<TSequence<T>>(this->clone_impl());
  }

  bool lower_inc() const;
  bool upper_inc() const;

  TemporalDuration duration() const override {
    return TemporalDuration::Sequence;
  };

  /**
   * Set of instants.
   */
  set<TInstant<T>> instants() const;
  Interpolation interpolation() const;
  duration_ms timespan() const override;
  set<Range<T>> getValues() const override;
  set<time_point> timestamps() const override;
  PeriodSet getTime() const override;
  Period period() const override;
  unique_ptr<TSequence<T>> shift(duration_ms const timedelta) const;
  TSequence<T> *shift_impl(duration_ms const timedelta) const override;
  bool intersectsTimestamp(time_point const datetime) const override;
  bool intersectsPeriod(Period const period) const override;

  friend istream &operator>>(istream &in, TSequence &sequence) {
    char c;

    // First we check for interpolation, if specified, else we stick with
    // default value (Stepwise for discrete base types, Linear otherwise)
    Interpolation interp =
        is_discrete_v<T> ? Interpolation::Stepwise : Interpolation::Linear;
    in >> std::ws;
    int pos = in.tellg();
    char prefix[6];
    in.read(prefix, 6);
    bool interp_specified = string(prefix) == "Interp";
    if (interp_specified) {
      consume(in, '=');
      std::string interp_string = read_until_one_of(in, "; \n\t");
      if (interp_string == "Stepwise") {
        interp = Interpolation::Stepwise;
      } else if (interp_string == "Linear") {
        if (is_discrete_v<T>) {
          throw invalid_argument(
              "Cannot assign linear interpolation to a discrete base type");
        }
        interp = Interpolation::Linear;
      } else {
        throw invalid_argument("Unsupported interpolation specified: " +
                               interp_string);
      }
      consume(in, ';');
    } else {
      in.seekg(pos);
    }

    c = consume_one_of(in, "[(");
    bool const lower_inc = c == '[';

    set<TInstant<T>> s = {};

    TInstant<T> instant;
    in >> instant;
    s.insert(instant);

    while (true) {
      in >> c;
      if (c != ',')
        break;
      in >> instant;
      s.insert(instant);
    }

    if (c != ']' && c != ')') {
      throw invalid_argument("Expected either a ']' or ')'");
    }
    bool const upper_inc = c == ']';

    sequence.m_instants = s;
    sequence.m_lower_inc = lower_inc;
    sequence.m_upper_inc = upper_inc;
    sequence.m_interpolation = interp;

    return in;
  }

  friend ostream &operator<<(ostream &os, TSequence<T> const &sequence) {
    if (sequence.interpolation() != default_interp_v<T>) {
      os << "Interp=" << sequence.interpolation() << ";";
    }

    bool first = true;
    os << (sequence.m_lower_inc ? "[" : "(");
    for (auto const &instant : sequence.instants()) {
      if (first)
        first = false;
      else
        os << ", ";
      os << instant;
    }
    os << (sequence.m_upper_inc ? "]" : ")");
    return os;
  }

private:
  set<TInstant<T>> m_instants;
  bool m_lower_inc;
  bool m_upper_inc;
  Interpolation m_interpolation;

  void validate() const;

  TSequence<T> *clone_impl() const override { return new TSequence<T>(*this); };
};

template class TSequence<bool>;
template class TSequence<int>;
template class TSequence<float>;
template class TSequence<string>;
template class TSequence<Geometry>;

#endif
