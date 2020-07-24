#include <iomanip>
#include <meos/types/temporal/TInstant.hpp>
#include <sstream>
#include <string>

template <typename BaseType> void TInstant<BaseType>::validate() {
  // None yet, check template specialization for Geometry for more validation
}

template <> void TInstant<Geometry>::validate() {
  // If the SRIDs is EXACTLY once, i.e, either on the object or on the
  // geometries, use it both places
  if (this->value.srid() * this->m_srid == 0) {
    if (this->m_srid != 0) {
      this->value = Geometry(this->value.x(), this->value.y(), this->m_srid);
    } else {
      this->m_srid = this->value.srid();
    }
  }

  // If the SRID was specified on both, ensure that they are the same value
  if (this->m_srid != this->value.srid()) {
    throw std::invalid_argument("Conflicting SRIDs provided. Given: " + to_string(this->srid())
                                + ", while Geometry contains: " + to_string(this->value.srid()));
  }
}

template <typename BaseType> TInstant<BaseType>::TInstant() {}

template <typename BaseType> TInstant<BaseType>::TInstant(BaseType value_, time_point t_)
    : value(value_), t(t_) {
  validate();
}

template <typename BaseType> TInstant<BaseType>::TInstant(pair<BaseType, time_point> p)
    : value(p.first), t(p.second) {
  validate();
}

template <typename BaseType> TInstant<BaseType>::TInstant(string const &value, string const &t) {
  stringstream lss(value);
  this->value = nextValue<BaseType>(lss);
  stringstream uss(t);
  this->t = nextTime(uss);
  validate();
}

template <typename BaseType> TInstant<BaseType>::TInstant(pair<string const, string const> p) {
  stringstream lss(p.first);
  this->value = nextValue<BaseType>(lss);
  stringstream uss(p.second);
  this->t = nextTime(uss);
  validate();
}

template <typename BaseType> TInstant<BaseType>::TInstant(string const &serialized) {
  stringstream ss(serialized);
  TInstant<BaseType> instant;
  ss >> instant;
  *this = instant;
  validate();
}

// Extra constructors for Geometry base type
// Note: Don't forget to instantiate the templates!

template <typename BaseType> template <typename B, typename is_geometry<B>::type *>
TInstant<BaseType>::TInstant(BaseType value_, time_point t_, int srid) : value(value_), t(t_) {
  this->m_srid = srid;
  validate();
}

template <typename BaseType> template <typename B, typename is_geometry<B>::type *>
TInstant<BaseType>::TInstant(pair<BaseType, time_point> p, int srid) : value(p.first), t(p.second) {
  this->m_srid = srid;
  validate();
}

template <typename BaseType> template <typename B, typename is_geometry<B>::type *>
TInstant<BaseType>::TInstant(string const &value, string const &t, int srid) {
  stringstream lss(value);
  this->value = nextValue<BaseType>(lss);
  stringstream uss(t);
  this->t = nextTime(uss);
  this->m_srid = srid;
  validate();
}

template <typename BaseType> template <typename B, typename is_geometry<B>::type *>
TInstant<BaseType>::TInstant(pair<string const, string const> p, int srid) {
  stringstream lss(p.first);
  this->value = nextValue<BaseType>(lss);
  stringstream uss(p.second);
  this->t = nextTime(uss);
  this->m_srid = srid;
  validate();
}

template <typename BaseType> template <typename B, typename is_geometry<B>::type *>
TInstant<BaseType>::TInstant(string const &serialized, int srid) {
  stringstream ss(serialized);
  TInstant<BaseType> instant;
  ss >> instant;
  *this = instant;
  this->m_srid = srid;
  validate();
}

template TInstant<Geometry>::TInstant(Geometry, time_point, int);
template TInstant<Geometry>::TInstant(pair<Geometry, time_point> p, int srid);
template TInstant<Geometry>::TInstant(string const &value, string const &t, int srid);
template TInstant<Geometry>::TInstant(pair<string const, string const> p, int srid);
template TInstant<Geometry>::TInstant(string const &serialized, int srid);

template <typename BaseType> template <typename B, typename is_geometry<B>::type *>
TInstant<BaseType> TInstant<BaseType>::with_srid(int srid) const {
  if (this->m_srid == srid) return *this;
  TInstant<BaseType> instant = *this;
  instant.value = Geometry(this->value.x(), this->value.y(), srid);
  instant.m_srid = srid;
  return instant;
}

template TInstant<Geometry> TInstant<Geometry>::with_srid(int srid) const;

template <typename BaseType>
int TInstant<BaseType>::compare_internal(Temporal<BaseType> const &other) const {
  if (this->duration() != other.duration()) {
    throw std::invalid_argument("Unsupported types for comparision");
  }

  TInstant<BaseType> const *that = dynamic_cast<TInstant<BaseType> const *>(&other);

  // Compare timestamps
  if (this->t < that->t) return -1;
  if (this->t > that->t) return 1;

  // Compare values
  if (this->value < that->value) return -1;
  if (this->value > that->value) return 1;

  // The two are equal
  return 0;
}

template <typename BaseType>
int TInstant<BaseType>::compare(Temporal<BaseType> const &other) const {
  return compare_internal(other);
}

template <> int TInstant<Geometry>::compare(Temporal<Geometry> const &other) const {
  // Compares timestamp and value
  int cmp = compare_internal(other);
  if (cmp != 0) {
    return cmp;
  }

  TInstant<Geometry> const *that = dynamic_cast<TInstant<Geometry> const *>(&other);

  // Compare SRID
  if (this->srid() < that->srid()) return -1;
  if (this->srid() > that->srid()) return 1;

  // The two are equal
  return 0;
}

template <typename BaseType> BaseType TInstant<BaseType>::getValue() const { return this->value; }

template <typename BaseType> time_point TInstant<BaseType>::getTimestamp() const { return this->t; }

template <typename BaseType> set<TInstant<BaseType>> TInstant<BaseType>::instants() const {
  return {*this};
}

template <typename BaseType> duration_ms TInstant<BaseType>::timespan() const {
  return duration_ms(0);
}

template <typename BaseType> set<Range<BaseType>> TInstant<BaseType>::getValues() const {
  return {Range<BaseType>(this->getValue(), this->getValue(), true, true)};
}

template <typename BaseType> set<time_point> TInstant<BaseType>::timestamps() const {
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
unique_ptr<TInstant<BaseType>> TInstant<BaseType>::shift(duration_ms const timedelta) const {
  return unique_ptr<TInstant<BaseType>>(this->shift_impl(timedelta));
}

template <typename BaseType> TInstant<BaseType> *TInstant<BaseType>::clone_impl() const {
  return new TInstant<BaseType>(this->value, this->t);
}

template <typename BaseType>
TInstant<BaseType> *TInstant<BaseType>::shift_impl(duration_ms const timedelta) const {
  return new TInstant<BaseType>(this->getValue(), this->getTimestamp() + timedelta);
}

template <typename BaseType>
bool TInstant<BaseType>::intersectsTimestamp(time_point const datetime) const {
  return datetime == this->t;
}

template <typename BaseType> bool TInstant<BaseType>::intersectsPeriod(Period const period) const {
  return period.contains_timestamp(this->t);
}

template <typename BaseType> istream &TInstant<BaseType>::read(istream &in) {
  this->value = nextValue<BaseType>(in);
  consume(in, '@');
  this->t = nextTime(in);
  return in;
}

template <> istream &TInstant<Geometry>::read(istream &in) {
  this->value = nextValue<Geometry>(in);
  consume(in, '@');
  this->t = nextTime(in);
  this->m_srid = this->value.srid();
  return in;
}

template <typename BaseType> ostream &TInstant<BaseType>::write(ostream &os, bool) const {
  // All base types except bool and Geometry follow this
  // For them, check the specialized implemenation
  os << this->getValue() << "@" << write_ISO8601_time(this->getTimestamp());
  return os;
}

template <> ostream &TInstant<bool>::write(ostream &os, bool) const {
  string serialized_value = this->getValue() ? "t" : "f";
  os << serialized_value << "@" << write_ISO8601_time(this->getTimestamp());
  return os;
}

template <> ostream &TInstant<Geometry>::write(ostream &os, bool with_srid) const {
  if (with_srid) {
    os << this->getValue() << "@" << write_ISO8601_time(this->getTimestamp());
  } else {
    os << this->getValue().toWKT() << "@" << write_ISO8601_time(this->getTimestamp());
  }
  return os;
}

template class TInstant<bool>;
template class TInstant<int>;
template class TInstant<float>;
template class TInstant<string>;
template class TInstant<Geometry>;
