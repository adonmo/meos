#include <iomanip>
#include <meos/types/temporal/TSequence.hpp>
#include <sstream>
#include <string>

template <typename BaseType> TSequence<BaseType>::TSequence() {}

template <typename BaseType>
TSequence<BaseType>::TSequence(set<TInstant<BaseType>> &instants, bool lower_inc, bool upper_inc,
                               Interpolation interpolation)
    : m_instants(instants),
      m_lower_inc(lower_inc),
      m_upper_inc(upper_inc),
      m_interpolation(interpolation) {
  validate();
}

template <typename BaseType>
TSequence<BaseType>::TSequence(set<string> const &instants, bool lower_inc, bool upper_inc,
                               Interpolation interpolation)
    : m_lower_inc(lower_inc), m_upper_inc(upper_inc), m_interpolation(interpolation) {
  TSequence<BaseType> instant_set;
  for (auto const &e : instants) m_instants.insert(TInstant<BaseType>(e));
  validate();
}

template <typename BaseType> TSequence<BaseType>::TSequence(string const &serialized) {
  stringstream ss(serialized);
  TSequence<BaseType> seq;
  ss >> seq;
  *this = seq;
  validate();
}

// Extra constructors for Geometry base type
// Note: Don't forget to instantiate the templates!

template <typename BaseType> template <typename B, typename is_geometry<B>::type *>
TSequence<BaseType>::TSequence(set<TInstant<BaseType>> &instants, bool lower_inc, bool upper_inc,
                               int srid, Interpolation interpolation)
    : m_instants(instants),
      m_lower_inc(lower_inc),
      m_upper_inc(upper_inc),
      m_interpolation(interpolation) {
  this->m_srid = srid;
  validate();
}

template <typename BaseType> template <typename B, typename is_geometry<B>::type *>
TSequence<BaseType>::TSequence(set<string> const &instants, bool lower_inc, bool upper_inc,
                               int srid, Interpolation interpolation)
    : m_lower_inc(lower_inc), m_upper_inc(upper_inc), m_interpolation(interpolation) {
  TSequence<BaseType> instant_set;
  for (auto const &e : instants) m_instants.insert(TInstant<BaseType>(e));
  this->m_srid = srid;
  validate();
}

template <typename BaseType> template <typename B, typename is_geometry<B>::type *>
TSequence<BaseType>::TSequence(string const &serialized, int srid) {
  stringstream ss(serialized);
  TSequence<BaseType> seq;
  ss >> seq;
  *this = seq;

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

template TSequence<Geometry>::TSequence(set<TInstant<Geometry>> &instants, bool lower_inc,
                                        bool upper_inc, int srid, Interpolation interpolation);
template TSequence<Geometry>::TSequence(set<string> const &instants, bool lower_inc, bool upper_inc,
                                        int srid, Interpolation interpolation);
template TSequence<Geometry>::TSequence(string const &serialized, int srid);

template <typename BaseType> template <typename B, typename is_geometry<B>::type *>
TSequence<BaseType> TSequence<BaseType>::with_srid(int srid) const {
  if (this->m_srid == srid) return *this;
  TSequence<BaseType> sequence = *this;
  set<TInstant<BaseType>> s;
  for (auto const &e : this->m_instants) s.insert(e.with_srid(srid));
  sequence.m_instants = s;
  sequence.m_srid = srid;
  return sequence;
}

template TSequence<Geometry> TSequence<Geometry>::with_srid(int srid) const;

template <typename BaseType>
TSequence<BaseType> TSequence<BaseType>::with_interp(Interpolation interpolation) const {
  if (this->m_interpolation == interpolation) return *this;
  TSequence<BaseType> sequence = *this;
  sequence.m_interpolation = interpolation;
  return sequence;
}

template TSequence<Geometry> TSequence<Geometry>::with_interp(Interpolation interpolation) const;

template <typename BaseType> void TSequence<BaseType>::validate_common() {
  size_t sz = this->m_instants.size();
  if (sz < 1) {
    throw invalid_argument("A sequence should have at least one instant");
  }

  if (this->m_interpolation == Interpolation::Linear && is_discrete_v<BaseType>) {
    throw invalid_argument("Cannot assign linear interpolation to a discrete base type");
  }

  // TODO
  // For stepwise interp and exclusive upper bound, last two values should match

  time_point start = this->startTimestamp();
  time_point end = this->endTimestamp();

  if (start > end) {
    throw invalid_argument("The lower bound must be less than or equal to the upper bound");
  }

  if (start == end) {
    bool both_inclusive = this->lower_inc() && this->upper_inc();
    if (!both_inclusive) {
      throw invalid_argument(
          "The lower and upper bounds must be inclusive for "
          "an instantaneous sequence");
    }
  }
}

template <typename BaseType> void TSequence<BaseType>::validate() {
  validate_common();
  // Check template specialization on Geometry for more validation
}

template <> void TSequence<Geometry>::validate() {
  validate_common();

  // If the SRIDs is EXACTLY once, i.e, either on the object or on the
  // geometries, use it both places
  // TODO improve - we are checking only first object. This logic is not ideal
  // when objects with a mix of different SRIDs are passed together
  TInstant<Geometry> instant = this->startInstant();
  if (instant.srid() * this->m_srid == 0) {
    if (this->m_srid != 0) {
      set<TInstant<Geometry>> _instants;
      for (TInstant<Geometry> const &instant : this->m_instants) {
        _instants.insert(instant.with_srid(this->m_srid));
      };
      this->m_instants = _instants;
    } else {
      this->m_srid = instant.srid();
    }
  }

  // All SRIDs must be equal
  for (TInstant<Geometry> const &instant : this->m_instants) {
    if (this->m_srid != instant.getValue().srid()) {
      throw std::invalid_argument("Conflicting SRIDs provided. Given: " + to_string(this->m_srid)
                                  + ", while Instant contains: " + to_string(instant.srid()));
    }
  }
}

template <typename BaseType>
int TSequence<BaseType>::compare_internal(Temporal<BaseType> const &other) const {
  if (this->duration() != other.duration()) {
    throw std::invalid_argument("Unsupported types for comparision");
  }

  TSequence<BaseType> const *that = dynamic_cast<TSequence<BaseType> const *>(&other);
  // Compare number of instants
  if (this->m_instants.size() < that->m_instants.size()) return -1;
  if (this->m_instants.size() > that->m_instants.size()) return 1;

  // Compare bounds
  // [ < (, ) < ]
  if ((this->m_lower_inc && !that->m_lower_inc) || (!this->m_upper_inc && that->m_upper_inc))
    return -1;
  // ( > [, ] > )
  if ((that->m_lower_inc && !this->m_lower_inc) || (!that->m_upper_inc && this->m_upper_inc))
    return 1;

  // Compare instant by instant
  auto lhs_instants = this->instants();
  auto rhs_instants = that->instants();
  auto lhs = lhs_instants.begin();
  auto rhs = rhs_instants.begin();
  while (lhs != lhs_instants.end()) {
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
int TSequence<BaseType>::compare(Temporal<BaseType> const &other) const {
  return compare_internal(other);
}

template <> int TSequence<Geometry>::compare(Temporal<Geometry> const &other) const {
  // Compare instants and bounds
  int cmp = compare_internal(other);
  if (cmp != 0) {
    return cmp;
  }

  TSequence<Geometry> const *that = dynamic_cast<TSequence<Geometry> const *>(&other);

  // Compare SRID
  if (this->srid() < that->srid()) return -1;
  if (this->srid() > that->srid()) return 1;

  // The two are equal
  return 0;
}

template <typename BaseType> bool TSequence<BaseType>::lower_inc() const {
  return this->m_lower_inc;
}
template <typename BaseType> bool TSequence<BaseType>::upper_inc() const {
  return this->m_upper_inc;
}

template <typename BaseType> set<TInstant<BaseType>> TSequence<BaseType>::instants() const {
  return this->m_instants;
}

template <typename BaseType> Interpolation TSequence<BaseType>::interpolation() const {
  return this->m_interpolation;
}

template <typename BaseType> duration_ms TSequence<BaseType>::timespan() const {
  return std::chrono::duration_cast<duration_ms>(this->endTimestamp() - this->startTimestamp());
}

template <typename BaseType> set<Range<BaseType>> TSequence<BaseType>::getValues() const {
  if (this->m_instants.size() == 0) return {};
  BaseType min = this->m_instants.begin()->getValue();
  BaseType max = this->m_instants.begin()->getValue();
  for (auto const &e : this->m_instants) {
    if (e.getValue() < min) {
      min = e.getValue();
    }
    if (e.getValue() > max) {
      max = e.getValue();
    }
  }
  return {Range<BaseType>(min, max, this->m_lower_inc, this->m_upper_inc)};
}

template <typename BaseType> set<time_point> TSequence<BaseType>::timestamps() const {
  set<time_point> s;
  for (auto const &e : this->m_instants) {
    s.insert(e.getTimestamp());
  }
  return s;
}

template <typename BaseType> PeriodSet TSequence<BaseType>::getTime() const {
  set<Period> s = {this->period()};
  return PeriodSet(s);
}

template <typename BaseType> Period TSequence<BaseType>::period() const {
  return Period(this->startTimestamp(), this->endTimestamp(), m_lower_inc, m_upper_inc);
}

template <typename BaseType>
unique_ptr<TSequence<BaseType>> TSequence<BaseType>::shift(duration_ms const timedelta) const {
  return unique_ptr<TSequence<BaseType>>(this->shift_impl(timedelta));
}

template <typename BaseType>
TSequence<BaseType> *TSequence<BaseType>::shift_impl(duration_ms const timedelta) const {
  set<TInstant<BaseType>> s;
  for (auto const &e : this->m_instants) {
    s.insert(TInstant<BaseType>(e.getValue(), e.getTimestamp() + timedelta));
  }
  return new TSequence<BaseType>(s, m_lower_inc, m_upper_inc);
}

template <typename BaseType>
bool TSequence<BaseType>::intersectsTimestamp(time_point const datetime) const {
  return this->period().contains_timestamp(datetime);
}

template <typename BaseType> bool TSequence<BaseType>::intersectsPeriod(Period const period) const {
  return this->period().overlap(period);
}

template <typename BaseType>
istream &TSequence<BaseType>::read_internal(istream &in, bool with_interp) {
  char c;

  Interpolation interp = is_discrete_v<BaseType> ? Interpolation::Stepwise : Interpolation::Linear;

  if (with_interp) {
    // First we check for interpolation, if specified, else we stick with
    // default value (Stepwise for discrete base types, Linear otherwise)
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
  }

  c = consume_one_of(in, "[(");
  bool const lower_inc = c == '[';

  set<TInstant<BaseType>> s = {};

  TInstant<BaseType> instant;
  in >> instant;
  s.insert(instant);

  while (true) {
    in >> c;
    if (c != ',') break;
    in >> instant;
    s.insert(instant);
  }

  if (c != ']' && c != ')') {
    throw invalid_argument("Expected either a ']' or ')'");
  }
  bool const upper_inc = c == ']';

  this->m_instants = s;
  this->m_lower_inc = lower_inc;
  this->m_upper_inc = upper_inc;
  this->m_interpolation = interp;

  return in;
}

template <typename BaseType>
istream &TSequence<BaseType>::read(istream &in, bool with_interp, bool) {
  read_internal(in, with_interp);
  return in;
}

template <> istream &TSequence<Geometry>::read(istream &in, bool with_interp, bool with_srid) {
  // First we check if SRID prefix is present, and if so, read it first
  int srid = 0;
  if (with_srid) {
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
  }

  read_internal(in, with_interp);
  this->m_srid = srid;
  return in;
}

template <typename BaseType>
ostream &TSequence<BaseType>::write_internal(ostream &os, bool with_interp) const {
  if (with_interp && this->interpolation() != default_interp_v<BaseType>) {
    os << "Interp=" << this->interpolation() << ";";
  }

  bool first = true;
  os << (this->m_lower_inc ? "[" : "(");
  for (auto const &instant : this->instants()) {
    if (first)
      first = false;
    else
      os << ", ";
    // We do not output SRID coming from instant
    instant.write(os, false);
  }
  os << (this->m_upper_inc ? "]" : ")");
  return os;
}

template <typename BaseType>
ostream &TSequence<BaseType>::write(ostream &os, bool with_interp, bool) const {
  write_internal(os, with_interp);
  return os;
}

template <>
ostream &TSequence<Geometry>::write(ostream &os, bool with_interp, bool with_srid) const {
  if (with_srid && this->srid() != 0) {
    os << "SRID=" << this->srid() << ";";
  }

  write_internal(os, with_interp);
  return os;
}
