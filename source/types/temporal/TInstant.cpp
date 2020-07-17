#include <iomanip>
#include <sstream>
#include <string>

#include <meos/types/temporal/TInstant.hpp>

template <typename BaseType> TInstant<BaseType>::TInstant() {}

template <typename BaseType>
TInstant<BaseType>::TInstant(BaseType value_, time_point t_)
    : value(value_), t(t_) {
  init();
}

template <typename BaseType>
TInstant<BaseType>::TInstant(pair<BaseType, time_point> p)
    : value(p.first), t(p.second) {
  init();
}

template <typename BaseType>
TInstant<BaseType>::TInstant(string const &value, string const &t) {
  stringstream lss(value);
  this->value = nextValue<BaseType>(lss);
  stringstream uss(t);
  this->t = nextTime(uss);
  init();
}

template <typename BaseType>
TInstant<BaseType>::TInstant(pair<string const, string const> p) {
  stringstream lss(p.first);
  this->value = nextValue<BaseType>(lss);
  stringstream uss(p.second);
  this->t = nextTime(uss);
  init();
}

template <typename BaseType>
TInstant<BaseType>::TInstant(string const &serialized) {
  stringstream ss(serialized);
  TInstant<BaseType> instant;
  ss >> instant;
  *this = instant;
  init();
}

// Extra constructors for Geometry base type
// Note: Don't forget to instantiate the templates!

template <typename BaseType>
template <typename B, typename is_geometry<B>::type *>
TInstant<BaseType>::TInstant(BaseType value_, time_point t_, int srid)
    : value(value_), t(t_) {
  this->m_srid = srid;
  init();
}

template <typename BaseType>
template <typename B, typename is_geometry<B>::type *>
TInstant<BaseType>::TInstant(pair<BaseType, time_point> p, int srid)
    : value(p.first), t(p.second) {
  this->m_srid = srid;
  init();
}

template <typename BaseType>
template <typename B, typename is_geometry<B>::type *>
TInstant<BaseType>::TInstant(string const &value, string const &t, int srid) {
  stringstream lss(value);
  this->value = nextValue<BaseType>(lss);
  stringstream uss(t);
  this->t = nextTime(uss);
  this->m_srid = srid;
  init();
}

template <typename BaseType>
template <typename B, typename is_geometry<B>::type *>
TInstant<BaseType>::TInstant(pair<string const, string const> p, int srid) {
  stringstream lss(p.first);
  this->value = nextValue<BaseType>(lss);
  stringstream uss(p.second);
  this->t = nextTime(uss);
  this->m_srid = srid;
  init();
}

template TInstant<Geometry>::TInstant(Geometry, time_point, int);
template TInstant<Geometry>::TInstant(pair<Geometry, time_point> p, int srid);
template TInstant<Geometry>::TInstant(string const &value, string const &t,
                                      int srid);
template TInstant<Geometry>::TInstant(pair<string const, string const> p,
                                      int srid);

template <typename BaseType> void TInstant<BaseType>::init() {
  setup_defaults();
  validate();
}

template <typename BaseType> void TInstant<BaseType>::setup_defaults() {
  // None yet, check specialization for Geometry
}

template <> void TInstant<Geometry>::setup_defaults() {
  // If EXACTLY one of two the SRIDs is 0, replace it with the non-zero SRID
  if (this->value.srid() * this->m_srid == 0) {
    if (this->m_srid != 0) {
      this->value = Geometry(this->value.x(), this->value.y(), this->m_srid);
    } else {
      this->m_srid = this->value.srid();
    }
  }
}

template <typename BaseType> void TInstant<BaseType>::validate() const {
  // None yet, check specialization for Geometry
}

template <> void TInstant<Geometry>::validate() const {
  if (this->srid() != this->value.srid()) {
    throw std::invalid_argument(
        "Conflicting SRIDs provided. Given: " + to_string(this->srid()) +
        ", which Geometry contains: " + to_string(this->value.srid()));
  }
}

template <typename BaseType>
int TInstant<BaseType>::compare_internal(
    Temporal<BaseType> const &other) const {
  if (this->duration() != other.duration()) {
    throw std::invalid_argument("Unsupported types for comparision");
  }

  TInstant<BaseType> const *that =
      dynamic_cast<TInstant<BaseType> const *>(&other);

  // Compare timestamps
  if (this->t < that->t)
    return -1;
  if (this->t > that->t)
    return 1;

  // Compare values
  if (this->value < that->value)
    return -1;
  if (this->value > that->value)
    return 1;

  // The two are equal
  return 0;
}

template <typename BaseType>
int TInstant<BaseType>::compare(Temporal<BaseType> const &other) const {
  return compare_internal(other);
}

template <>
int TInstant<Geometry>::compare(Temporal<Geometry> const &other) const {
  // Compares timestamp and value
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

template <typename BaseType> BaseType TInstant<BaseType>::getValue() const {
  return this->value;
}

template <typename BaseType>
time_point TInstant<BaseType>::getTimestamp() const {
  return this->t;
}

template <typename BaseType>
set<TInstant<BaseType>> TInstant<BaseType>::instants() const {
  return {*this};
}

template <typename BaseType> duration_ms TInstant<BaseType>::timespan() const {
  return duration_ms(0);
}

template <typename BaseType>
set<Range<BaseType>> TInstant<BaseType>::getValues() const {
  return {Range<BaseType>(this->getValue(), this->getValue(), true, true)};
}

template <typename BaseType>
set<time_point> TInstant<BaseType>::timestamps() const {
  return {getTimestamp()};
}

template <typename BaseType> PeriodSet TInstant<BaseType>::getTime() const {
  set<Period> s = {this->period()};
  return PeriodSet(s);
}

template <typename BaseType> Period TInstant<BaseType>::period() const {
  return Period(this->getTimestamp(), this->getTimestamp(), true, true);
}

template <typename BaseType>
unique_ptr<TInstant<BaseType>>
TInstant<BaseType>::shift(duration_ms const timedelta) const {
  return unique_ptr<TInstant<BaseType>>(this->shift_impl(timedelta));
}

template <typename BaseType>
TInstant<BaseType> *TInstant<BaseType>::clone_impl() const {
  return new TInstant<BaseType>(this->value, this->t);
}

template <typename BaseType>
TInstant<BaseType> *
TInstant<BaseType>::shift_impl(duration_ms const timedelta) const {
  return new TInstant<BaseType>(this->getValue(),
                                this->getTimestamp() + timedelta);
}

template <typename BaseType>
bool TInstant<BaseType>::intersectsTimestamp(time_point const datetime) const {
  return datetime == this->t;
}

template <typename BaseType>
bool TInstant<BaseType>::intersectsPeriod(Period const period) const {
  return period.contains_timestamp(this->t);
}

template <typename BaseType> istream &TInstant<BaseType>::read(istream &in) {
  this->value = nextValue<BaseType>(in);
  consume(in, '@');
  this->t = nextTime(in);
  return in;
}

template <> istream &TInstant<Geometry>::read(istream &in) {
  int srid = 0;

  // In the case of BaseType being Geometry,
  // we first check if a SRID prefix is present
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

  this->value = nextValue<Geometry>(in);
  consume(in, '@');
  this->t = nextTime(in);
  this->m_srid = srid;

  return in;
}

template <typename BaseType>
ostream &TInstant<BaseType>::write(ostream &os) const {
  // All base types except boolean follow this
  // For bool, check the specialized implemenation
  os << this->getValue() << "@" << write_ISO8601_time(this->getTimestamp());
  return os;
}

template <> ostream &TInstant<bool>::write(ostream &os) const {
  string serialized_value = this->getValue() ? "t" : "f";
  os << serialized_value << "@" << write_ISO8601_time(this->getTimestamp());
  return os;
}
