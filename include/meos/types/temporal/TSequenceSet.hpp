#ifndef MEOS_TYPES_TEMPORAL_TSEQUENCESET_HPP
#define MEOS_TYPES_TEMPORAL_TSEQUENCESET_HPP

#include <set>

#include <meos/io/utils.hpp>
#include <meos/types/geom/Geometry.hpp>
#include <meos/types/temporal/Interpolation.hpp>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantFunctions.hpp>
#include <meos/types/temporal/TSequence.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <meos/types/temporal/TemporalComparators.hpp>
#include <meos/util/serializing.hpp>

using namespace std;

using time_point = std::chrono::system_clock::time_point;
using duration_ms = std::chrono::milliseconds;

template <typename T = float>
class TSequenceSet : public Temporal<T>,
                     public TemporalComparators<TSequenceSet<T>>,
                     public TInstantFunctions<TSequenceSet<T>, TInstant<T>, T> {
public:
  TSequenceSet();
  TSequenceSet(set<TSequence<T>> const &sequences,
               Interpolation interpolation = default_interp_v<T>);
  TSequenceSet(set<string> const &sequences,
               Interpolation interpolation = default_interp_v<T>);
  TSequenceSet(string const &serialized);

  int compare(Temporal<T> const &other) const override;

  unique_ptr<TSequenceSet<T>> clone() const {
    return std::unique_ptr<TSequenceSet<T>>(this->clone_impl());
  }

  TemporalDuration duration() const override {
    return TemporalDuration::SequenceSet;
  };

  Interpolation interpolation() const;

  /**
   * Set of sequences.
   */
  set<TSequence<T>> sequences() const;

  /**
   * Number of distinct sequences.
   */
  size_t numSequences() const;

  /**
   * Start sequence.
   */
  TSequence<T> startSequence() const;

  /**
   * End sequence.
   */
  TSequence<T> endSequence() const;

  /**
   * N-th distinct sequence.
   */
  TSequence<T> sequenceN(size_t n) const;

  /**
   * Set of instants.
   */
  set<TInstant<T>> instants() const;

  duration_ms timespan() const override;
  set<Range<T>> getValues() const override;
  set<time_point> timestamps() const override;
  PeriodSet getTime() const override;
  Period period() const override;
  unique_ptr<TSequenceSet<T>> shift(duration_ms const timedelta) const;
  TSequenceSet<T> *shift_impl(duration_ms const timedelta) const override;
  bool intersectsTimestamp(time_point const datetime) const override;
  bool intersectsPeriod(Period const period) const override;

  friend istream &operator>>(istream &in, TSequenceSet<T> &sequence_set) {
    char c;

    // First we check for interpolation, if specified, else we stick with
    // default value (Stepwise for discrete base types, Linear otherwise)
    Interpolation interp =
        is_discrete_v<T> ? Interpolation::Stepwise : Interpolation::Linear;
    in >> std::ws;
    int pos = in.tellg();
    char prefix[6];
    in.read(prefix, 6);
    bool interp_specified = string(prefix, 6) == "Interp";
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

    consume(in, '{');

    set<TSequence<T>> s = {};

    // When reading sequences, we assume no "Interp=..;" prefix is present
    // We then create sequence objects with the sequence set's interp value
    // TODO - code can be more cleaner / efficient
    TSequence<T> seq;
    seq.read(in, false);
    set<TInstant<T>> instants = seq.instants();
    s.insert(TSequence<T>(instants, seq.lower_inc(), seq.upper_inc(), interp));

    while (true) {
      in >> c;
      if (c != ',')
        break;
      seq.read(in, false);
      instants = seq.instants();
      s.insert(
          TSequence<T>(instants, seq.lower_inc(), seq.upper_inc(), interp));
    }

    if (c != '}') {
      throw invalid_argument("Expected '}'");
    }

    sequence_set.m_sequences = s;
    sequence_set.m_interpolation = interp;

    return in;
  }

  friend ostream &operator<<(ostream &os, TSequenceSet const &sequence_set) {
    if (sequence_set.interpolation() != default_interp_v<T>) {
      os << "Interp=" << sequence_set.interpolation() << ";";
    }

    bool first = true;
    os << "{";
    for (auto sequence : sequence_set.sequences()) {
      if (first)
        first = false;
      else
        os << ", ";
      sequence.write(os, false);
    }
    os << "}";
    return os;
  }

private:
  set<TSequence<T>> m_sequences;
  Interpolation m_interpolation;

  void validate() const;

  TSequenceSet<T> *clone_impl() const override {
    return new TSequenceSet<T>(*this);
  };
};

template class TSequenceSet<bool>;
template class TSequenceSet<int>;
template class TSequenceSet<float>;
template class TSequenceSet<string>;
template class TSequenceSet<Geometry>;

#endif
