#include <iomanip>
#include <meos/types/temporal/Interpolation.hpp>
#include <meos/types/temporal/TSequenceSet.hpp>
#include <sstream>
#include <string>

template <typename BaseType> void TSequenceSet<BaseType>::validate() {
  validate_common();
  // Check template specialization on Geometry for more validation
}

template <> void TSequenceSet<Geometry>::validate() {
  validate_common();

  // If the SRIDs is AT MOST once, i.e, either on the object or on the
  // sequences, use it both places
  // TODO improve - we are checking only first object. This logic is not ideal
  // when objects with a mix of different SRIDs are passed together
  TSequence<Geometry> sequence = this->startSequence();
  if (sequence.srid() * this->m_srid == 0) {
    if (this->m_srid != 0) {
      set<TSequence<Geometry>> _sequences;
      for (TSequence<Geometry> const &sequence : this->m_sequences) {
        _sequences.insert(sequence.with_srid(this->m_srid));
      };
      this->m_sequences = _sequences;
    } else {
      this->m_srid = sequence.srid();
    }
  }

  // All SRIDs must be equal
  for (TSequence<Geometry> const &sequence : this->m_sequences) {
    if (this->m_srid != sequence.srid()) {
      throw std::invalid_argument("Conflicting SRIDs provided. Given: " + to_string(this->m_srid)
                                  + ", while Sequence contains: " + to_string(sequence.srid()));
    }
  }
}

template <typename BaseType> TSequenceSet<BaseType>::TSequenceSet() {}

template <typename BaseType>
TSequenceSet<BaseType>::TSequenceSet(set<TSequence<BaseType>> const &sequences,
                                     Interpolation interpolation)
    : m_sequences(sequences), m_interpolation(interpolation) {
  validate();
}

template <typename BaseType>
TSequenceSet<BaseType>::TSequenceSet(set<string> const &ss, Interpolation interpolation)
    : m_interpolation(interpolation) {
  for (auto const &e : ss) this->m_sequences.insert(TSequence<BaseType>(e));
  validate();
}

template <typename BaseType> TSequenceSet<BaseType>::TSequenceSet(string const &serialized) {
  stringstream ss(serialized);
  TSequenceSet<BaseType> sequence_set;
  ss >> sequence_set;
  *this = sequence_set;
  validate();
}

// Extra constructors for Geometry base type
// Note: Don't forget to instantiate the templates!

template <typename BaseType> template <typename B, typename is_geometry<B>::type *>
TSequenceSet<BaseType>::TSequenceSet(set<TSequence<BaseType>> const &sequences, int srid,
                                     Interpolation interpolation)
    : m_sequences(sequences), m_interpolation(interpolation) {
  this->m_srid = srid;
  validate();
}

template <typename BaseType> template <typename B, typename is_geometry<B>::type *>
TSequenceSet<BaseType>::TSequenceSet(set<string> const &ss, int srid, Interpolation interpolation)
    : m_interpolation(interpolation) {
  for (auto const &e : ss) this->m_sequences.insert(TSequence<BaseType>(e));
  this->m_srid = srid;
  validate();
}

template <typename BaseType> template <typename B, typename is_geometry<B>::type *>
TSequenceSet<BaseType>::TSequenceSet(string const &serialized, int srid) {
  stringstream ss(serialized);
  TSequenceSet<BaseType> sequence_set;
  ss >> sequence_set;
  *this = sequence_set;

  // When both string and int have non zero SRIDs, it gets a bit tricky
  if (srid * this->m_srid != 0) {
    // That is because of possibility of conflicting SRID
    if (this->m_srid != srid) {
      throw std::invalid_argument("Conflicting SRIDs provided. Given: " + to_string(srid)
                                  + ", while Geometry contains: " + to_string(this->m_srid));
    }
  }

  // We need set SRID a bit carefully here, as indvidual instant inside the set
  // need not necessarily specify the SRID when a set is serialized as a whole
  if (srid != 0) {
    this->m_srid = srid;
  }

  validate();
}

// clang-format off
template TSequenceSet<Geometry>::TSequenceSet(set<TSequence<Geometry>> const &sequences, int srid, Interpolation interpolation);
template TSequenceSet<Geometry>::TSequenceSet(set<string> const &sequences, int srid, Interpolation interpolation);
template TSequenceSet<Geometry>::TSequenceSet(string const &serialized, int srid);
// clang-format on

template <typename BaseType> void TSequenceSet<BaseType>::validate_common() {
  if (this->m_sequences.size() < 1) {
    throw invalid_argument("A sequence should have at least one temporal unit");
  }

  if (this->m_interpolation == Interpolation::Linear && is_discrete_v<BaseType>) {
    throw invalid_argument("Cannot assign linear interpolation to a discrete base type");
  }

  // If the interpolation is non-default AT MOST once, i.e, either on the object
  // or on the sequences, use it both places
  // TODO improve - we are checking only first object. This logic is not ideal
  // when objects with a mix of different interpolations are passed together
  TSequence<BaseType> s = this->startSequence();
  bool non_defult_interp_on_first_seq = s.interpolation() != default_interp_v<BaseType>;
  bool non_defult_interp_on_seq_set = this->m_interpolation != default_interp_v<BaseType>;
  bool non_default_at_most_once = !(non_defult_interp_on_first_seq && non_defult_interp_on_seq_set);
  if (non_default_at_most_once) {
    if (non_defult_interp_on_seq_set) {
      set<TSequence<BaseType>> _sequences;
      for (TSequence<BaseType> const &sequence : this->m_sequences) {
        _sequences.insert(sequence.with_interp(this->m_interpolation));
      };
      this->m_sequences = _sequences;
    } else {
      this->m_interpolation = s.interpolation();
    }
  }

  for (auto const &e : this->m_sequences) {
    if (this->m_interpolation != e.interpolation()) {
      throw invalid_argument("All sequences should have the same interpolation");
    }
  }

  // TODO
  // All sequences should be non overlapping
}

template <typename BaseType>
int TSequenceSet<BaseType>::compare_internal(Temporal<BaseType> const &other) const {
  if (this->duration() != other.duration()) {
    throw std::invalid_argument("Unsupported types for comparision");
  }

  TSequenceSet<BaseType> const *that = dynamic_cast<TSequenceSet<BaseType> const *>(&other);
  // Compare number of sequences
  if (this->m_sequences.size() < that->m_sequences.size()) return -1;
  if (this->m_sequences.size() > that->m_sequences.size()) return 1;

  // Compare sequence by sequence
  auto lhs_sequences = this->sequences();
  auto rhs_sequences = that->sequences();
  auto lhs = lhs_sequences.begin();
  auto rhs = rhs_sequences.begin();
  while (lhs != lhs_sequences.end()) {
    if (*lhs < *rhs) return -1;
    if (*lhs > *rhs) return 1;
    lhs++;
    rhs++;
  }

  // Compare Interpolation
  if (this->interpolation() < that->interpolation()) return -1;
  if (this->interpolation() > that->interpolation()) return 1;

  // The two are equal
  return 0;
}

template <typename BaseType>
int TSequenceSet<BaseType>::compare(Temporal<BaseType> const &other) const {
  return compare_internal(other);
}

template <> int TSequenceSet<Geometry>::compare(Temporal<Geometry> const &other) const {
  // Compare sequences and interpolation
  int cmp = compare_internal(other);
  if (cmp != 0) {
    return cmp;
  }

  TSequenceSet<Geometry> const *that = dynamic_cast<TSequenceSet<Geometry> const *>(&other);

  // Compare SRID
  if (this->srid() < that->srid()) return -1;
  if (this->srid() > that->srid()) return 1;

  // The two are equal
  return 0;
}

template <typename BaseType> Interpolation TSequenceSet<BaseType>::interpolation() const {
  return this->m_interpolation;
}

template <typename BaseType> set<TSequence<BaseType>> TSequenceSet<BaseType>::sequences() const {
  return this->m_sequences;
}

/**
 * Number of distinct sequences.
 */
template <typename BaseType> size_t TSequenceSet<BaseType>::numSequences() const {
  return this->m_sequences.size();
}

/**
 * Start sequence.
 */
template <typename BaseType> TSequence<BaseType> TSequenceSet<BaseType>::startSequence() const {
  auto s = this->m_sequences;
  if (s.size() <= 0) {
    throw "At least one sequence expected";
  }
  return *s.begin();
}

/**
 * End sequence.
 */
template <typename BaseType> TSequence<BaseType> TSequenceSet<BaseType>::endSequence() const {
  auto s = this->m_sequences;
  if (s.size() <= 0) {
    throw "At least one sequence expected";
  }
  return *s.rbegin();
}

/**
 * N-th distinct sequence.
 */
template <typename BaseType> TSequence<BaseType> TSequenceSet<BaseType>::sequenceN(size_t n) const {
  auto s = this->m_sequences;
  if (s.size() <= n) {
    throw "At least " + std::to_string(n) + " sequence(s) expected";
  }
  return *next(s.begin(), n);
}

template <typename BaseType> set<TInstant<BaseType>> TSequenceSet<BaseType>::instants() const {
  set<TInstant<BaseType>> s;
  for (auto const &e : this->m_sequences) {
    for (auto const &e : e.instants()) {
      s.insert(e);
    }
  }
  return s;
}

template <typename BaseType> duration_ms TSequenceSet<BaseType>::timespan() const {
  duration_ms tspan(0);
  for (auto const &e : this->m_sequences) tspan += e.timespan();
  return tspan;
}

template <typename BaseType> set<Range<BaseType>> TSequenceSet<BaseType>::getValues() const {
  set<Range<BaseType>> s;
  for (auto const &e : this->m_sequences) {
    for (auto const &e : e.getValues()) {
      s.insert(e);
    }
  }
  return s;
}

template <typename BaseType> set<time_point> TSequenceSet<BaseType>::timestamps() const {
  set<time_point> s;
  for (auto const &e : this->m_sequences) {
    for (auto const &e : e.timestamps()) {
      s.insert(e);
    }
  }
  return s;
}

template <typename BaseType> PeriodSet TSequenceSet<BaseType>::getTime() const {
  set<Period> s;
  for (auto const &e : sequences()) {
    s.insert(e.period());
  }
  return PeriodSet(s);
}

template <typename BaseType> Period TSequenceSet<BaseType>::period() const {
  return Period(this->startTimestamp(), this->endTimestamp(), true, true);
}

template <typename BaseType> unique_ptr<TSequenceSet<BaseType>> TSequenceSet<BaseType>::shift(
    duration_ms const timedelta) const {
  return unique_ptr<TSequenceSet<BaseType>>(this->shift_impl(timedelta));
}

template <typename BaseType>
TSequenceSet<BaseType> *TSequenceSet<BaseType>::shift_impl(duration_ms const timedelta) const {
  set<TSequence<BaseType>> s;
  for (auto const &e : this->m_sequences) {
    s.insert(*e.shift(timedelta).get());
  }
  return new TSequenceSet<BaseType>(s);
}

template <typename BaseType>
bool TSequenceSet<BaseType>::intersectsTimestamp(time_point const datetime) const {
  for (auto const &t : this->timestamps()) {
    if (t == datetime) {
      return true;
    }
  }
  return false;
}

template <typename BaseType>
bool TSequenceSet<BaseType>::intersectsPeriod(Period const period) const {
  for (auto const &t : this->timestamps()) {
    if (period.contains_timestamp(t)) {
      return true;
    }
  }
  return false;
}

template <typename BaseType> istream &TSequenceSet<BaseType>::read_internal(istream &in) {
  char c;

  // First we check for interpolation, if specified, else we stick with
  // default value (Stepwise for discrete base types, Linear otherwise)
  Interpolation interp = is_discrete_v<BaseType> ? Interpolation::Stepwise : Interpolation::Linear;
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
      if (is_discrete_v<BaseType>) {
        throw invalid_argument("Cannot assign linear interpolation to a discrete base type");
      }
      interp = Interpolation::Linear;
    } else {
      throw invalid_argument("Unsupported interpolation specified: " + interp_string);
    }
    consume(in, ';');
  } else {
    in.seekg(pos);
  }

  consume(in, '{');

  set<TSequence<BaseType>> s = {};

  TSequence<BaseType> seq;
  in >> seq;
  seq.validate();  // TODO can we avoid this?
  s.insert(seq);

  while (true) {
    in >> c;
    if (c != ',') break;
    in >> seq;
    seq.validate();
    s.insert(seq);
  }

  if (c != '}') {
    throw invalid_argument("Expected '}'");
  }

  this->m_sequences = s;
  this->m_interpolation = interp;

  return in;
}

template <typename BaseType> istream &TSequenceSet<BaseType>::read(istream &in) {
  read_internal(in);
  return in;
}

template <> istream &TSequenceSet<Geometry>::read(istream &in) {
  // First we check if SRID prefix is present, and if so, read it first
  int srid = 0;
  in >> std::ws;
  int pos = in.tellg();
  char prefix[4];
  in.read(prefix, 4);
  bool srid_specified = string(prefix, 4) == "SRID";
  if (srid_specified) {
    consume(in, '=');
    in >> srid;
    consume(in, ';');
  } else {
    in.seekg(pos);
  }

  read_internal(in);
  this->m_srid = srid;
  return in;
}

template <typename BaseType> ostream &TSequenceSet<BaseType>::write_internal(ostream &os) const {
  if (this->m_interpolation != default_interp_v<BaseType>) {
    os << "Interp=" << this->m_interpolation << ";";
  }

  bool first = true;
  os << "{";
  for (auto sequence : this->m_sequences) {
    if (first)
      first = false;
    else
      os << ", ";
    // We do not output SRID/Interp coming from sequence
    sequence.write(os, false, false);
  }
  os << "}";
  return os;
}

template <typename BaseType> ostream &TSequenceSet<BaseType>::write(ostream &os) const {
  write_internal(os);
  return os;
}

template <> ostream &TSequenceSet<Geometry>::write(ostream &os) const {
  if (this->srid() != 0) {
    os << "SRID=" << this->srid() << ";";
  }

  write_internal(os);
  return os;
}

template class TSequenceSet<bool>;
template class TSequenceSet<int>;
template class TSequenceSet<float>;
template class TSequenceSet<string>;
template class TSequenceSet<Geometry>;
