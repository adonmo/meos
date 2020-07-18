#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>

#include <meos/types/temporal/TInstantSet.hpp>

template <typename BaseType> TInstantSet<BaseType>::TInstantSet() {}

template <typename BaseType>
TInstantSet<BaseType>::TInstantSet(set<TInstant<BaseType>> const &instants)
    : m_instants(instants) {
  validate();
}

template <typename BaseType>
TInstantSet<BaseType>::TInstantSet(set<string> const &instants) {
  for (auto const &e : instants)
    this->m_instants.insert(TInstant<BaseType>(e));
  validate();
}

template <typename BaseType>
TInstantSet<BaseType>::TInstantSet(string const &serialized) {
  stringstream ss(serialized);
  TInstantSet<BaseType> instant_set;
  ss >> instant_set;
  *this = instant_set;
  validate();
}

// Extra constructors for Geometry base type
// Note: Don't forget to instantiate the templates!

template <typename BaseType>
template <typename B, typename is_geometry<B>::type *>
TInstantSet<BaseType>::TInstantSet(set<TInstant<BaseType>> const &instants,
                                   int srid)
    : m_instants(instants) {
  this->m_srid = srid;
  validate();
}

template <typename BaseType>
template <typename B, typename is_geometry<B>::type *>
TInstantSet<BaseType>::TInstantSet(set<string> const &instants, int srid) {
  for (auto const &e : instants)
    this->m_instants.insert(TInstant<BaseType>(e));
  this->m_srid = srid;
  validate();
}

template <typename BaseType>
template <typename B, typename is_geometry<B>::type *>
TInstantSet<BaseType>::TInstantSet(string const &serialized, int srid) {
  stringstream ss(serialized);
  TInstantSet<BaseType> instant_set;
  ss >> instant_set;
  *this = instant_set;

  // When both string and int have non zero SRIDs, it gets a bit tricky
  if (srid * this->m_srid != 0) {
    // That is because of possibility of conflicting SRID
    if (this->m_srid != srid) {
      throw std::invalid_argument(
          "Conflicting SRIDs provided. Given: " + to_string(srid) +
          ", while Geometry contains: " + to_string(this->m_srid));
    }
  }

  // We need set SRID a bit carefully here, as indvidual instant inside the set
  // need not necessarily specify the SRID when a set is serialized as a whole
  if (srid != 0) {
    this->m_srid = srid;
  }

  validate();
}

template TInstantSet<Geometry>::TInstantSet(
    set<TInstant<Geometry>> const &instants, int srid);
template TInstantSet<Geometry>::TInstantSet(set<string> const &instants,
                                            int srid);
template TInstantSet<Geometry>::TInstantSet(string const &serialized, int srid);

template <typename BaseType> void TInstantSet<BaseType>::validate_common() {
  size_t sz = this->m_instants.size();
  if (sz < 1) {
    throw invalid_argument("A sequence should have at least one instant");
  }
}

template <typename BaseType> void TInstantSet<BaseType>::validate() {
  validate_common();
  // Check template specialization on Geometry for more validation
}

template <> void TInstantSet<Geometry>::validate() {
  validate_common();

  // If the SRIDs is EXACTLY once, i.e, either on the object or on the
  // geometries, use it both places
  Geometry g = this->startValue();
  if (g.srid() * this->m_srid == 0) {
    if (this->m_srid != 0) {
      set<TInstant<Geometry>> _instants;
      for (TInstant<Geometry> const &instant : this->m_instants) {
        _instants.insert(TInstant<Geometry>(
            instant.getValue(), instant.getTimestamp(), this->m_srid));
      };
      this->m_instants = _instants;
    } else {
      this->m_srid = g.srid();
    }
  }

  // All SRIDs must be equal
  for (TInstant<Geometry> const &instant : this->m_instants) {
    if (this->m_srid != instant.getValue().srid()) {
      throw std::invalid_argument(
          "Conflicting SRIDs provided. Given: " + to_string(this->m_srid) +
          ", while Geometry contains: " + to_string(g.srid()));
    }
  }
}

template <typename BaseType>
int TInstantSet<BaseType>::compare_internal(
    Temporal<BaseType> const &other) const {
  if (this->duration() != other.duration()) {
    throw std::invalid_argument("Unsupported types for comparision");
  }

  TInstantSet<BaseType> const *that =
      dynamic_cast<TInstantSet<BaseType> const *>(&other);
  // Compare number of instants
  if (this->m_instants.size() < that->m_instants.size())
    return -1;
  if (this->m_instants.size() > that->m_instants.size())
    return 1;

  // Compare instant by instant
  auto lhs_instants = this->instants();
  auto rhs_instants = that->instants();
  auto lhs = lhs_instants.begin();
  auto rhs = rhs_instants.begin();
  while (lhs != lhs_instants.end()) {
    if (*lhs < *rhs)
      return -1;
    if (*lhs > *rhs)
      return 1;
    lhs++;
    rhs++;
  }

  // The two are equal
  return 0;
}

template <typename BaseType>
int TInstantSet<BaseType>::compare(Temporal<BaseType> const &other) const {
  return compare_internal(other);
}

template <>
int TInstantSet<Geometry>::compare(Temporal<Geometry> const &other) const {
  // Compare instants
  int cmp = compare_internal(other);
  if (cmp != 0) {
    return cmp;
  }

  TInstant<Geometry> const *that =
      dynamic_cast<TInstant<Geometry> const *>(&other);

  // Compare SRID
  if (this->srid() < that->srid())
    return -1;
  if (this->srid() > that->srid())
    return 1;

  // The two are equal
  return 0;
}

template <typename BaseType>
set<TInstant<BaseType>> TInstantSet<BaseType>::instants() const {
  return this->m_instants;
}

template <typename BaseType>
duration_ms TInstantSet<BaseType>::timespan() const {
  return duration_ms(0);
}

template <typename BaseType>
set<Range<BaseType>> TInstantSet<BaseType>::getValues() const {
  set<Range<BaseType>> s;
  for (auto const &e : this->m_instants) {
    s.insert(Range<BaseType>(e.getValue(), e.getValue(), true, true));
  }
  return s;
}

template <typename BaseType>
set<time_point> TInstantSet<BaseType>::timestamps() const {
  set<time_point> s;
  for (auto const &e : this->m_instants) {
    s.insert(e.getTimestamp());
  }
  return s;
}

template <typename BaseType> PeriodSet TInstantSet<BaseType>::getTime() const {
  set<Period> s;
  for (auto const &e : instants()) {
    s.insert(e.period());
  }
  return PeriodSet(s);
}

template <typename BaseType> Period TInstantSet<BaseType>::period() const {
  return Period(this->startTimestamp(), this->endTimestamp(), true, true);
}

template <typename BaseType>
unique_ptr<TInstantSet<BaseType>>
TInstantSet<BaseType>::shift(duration_ms const timedelta) const {
  return unique_ptr<TInstantSet<BaseType>>(this->shift_impl(timedelta));
}

template <typename BaseType>
TInstantSet<BaseType> *
TInstantSet<BaseType>::shift_impl(duration_ms const timedelta) const {
  set<TInstant<BaseType>> s;
  for (auto const &e : this->m_instants) {
    s.insert(TInstant<BaseType>(e.getValue(), e.getTimestamp() + timedelta));
  }
  return new TInstantSet<BaseType>(s);
}

template <typename BaseType>
bool TInstantSet<BaseType>::intersectsTimestamp(
    time_point const datetime) const {
  for (auto const &t : this->timestamps()) {
    if (t == datetime) {
      return true;
    }
  }
  return false;
}

template <typename BaseType>
bool TInstantSet<BaseType>::intersectsPeriod(Period const period) const {
  for (auto const &t : this->timestamps()) {
    if (period.contains_timestamp(t)) {
      return true;
    }
  }
  return false;
}

template <typename BaseType>
istream &TInstantSet<BaseType>::read_internal(istream &in) {
  char c;

  consume(in, '{');

  set<TInstant<BaseType>> s = {};

  TInstant<BaseType> instant;
  in >> instant;
  s.insert(instant);

  while (true) {
    in >> c;
    if (c != ',')
      break;
    in >> instant;
    s.insert(instant);
  }

  if (c != '}') {
    throw invalid_argument("Expected '}'");
  }

  this->m_instants = s;

  return in;
}

template <typename BaseType> istream &TInstantSet<BaseType>::read(istream &in) {
  read_internal(in);
  return in;
}

template <> istream &TInstantSet<Geometry>::read(istream &in) {
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

template <typename BaseType>
ostream &TInstantSet<BaseType>::write_internal(ostream &os) const {
  bool first = true;
  os << "{";
  for (auto instant : this->m_instants) {
    if (first)
      first = false;
    else
      os << ", ";
    // We do not output SRID coming from instant
    instant.write(os, false);
  }
  os << "}";
  return os;
}

template <typename BaseType>
ostream &TInstantSet<BaseType>::write(ostream &os) const {
  write_internal(os);
  return os;
}

template <> ostream &TInstantSet<Geometry>::write(ostream &os) const {
  if (this->srid() != 0) {
    os << "SRID=" << this->srid() << ";";
  }

  write_internal(os);
  return os;
}
