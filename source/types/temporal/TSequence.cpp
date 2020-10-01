#include <iomanip>
#include <meos/types/temporal/TSequence.hpp>
#include <sstream>
#include <string>

namespace meos {
using namespace std;

template <typename BaseType> void TSequence<BaseType>::validate() {
  validate_common();
  // Check template specialization on Geometry for more validation
}

template <> void TSequence<GeomPoint>::validate() {
  validate_common();

  // If the SRIDs is EXACTLY once, i.e, either on the object or on the
  // geometries, use it both places
  // TODO improve - we are checking only first object. This logic is not ideal
  // when objects with a mix of different SRIDs are passed together
  TInstant<GeomPoint> instant = this->startInstant();
  if (instant.srid() * this->m_srid == 0) {
    if (this->m_srid != 0) {
      set<TInstant<GeomPoint>> _instants;
      for (TInstant<GeomPoint> const &instant : this->m_instants) {
        _instants.insert(instant.with_srid(this->m_srid));
      };
      this->m_instants = _instants;
    } else {
      this->m_srid = instant.srid();
    }
  }

  // All SRIDs must be equal
  for (TInstant<GeomPoint> const &instant : this->m_instants) {
    if (this->m_srid != instant.getValue().srid()) {
      throw std::invalid_argument("Conflicting SRIDs provided. Given: " + to_string(this->m_srid)
                                  + ", while Instant contains: " + to_string(instant.srid()));
    }
  }
}

template <typename BaseType> TSequence<BaseType>::TSequence() {}

template <typename BaseType>
TSequence<BaseType>::TSequence(set<TInstant<BaseType>> &instants, bool lower_inc, bool upper_inc,
                               Interpolation interpolation)
    : TemporalSet<BaseType>(instants),
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
  for (auto const &e : instants) this->m_instants.insert(TInstant<BaseType>(e));
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
    : TemporalSet<BaseType>(instants),
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
  for (auto const &e : instants) this->m_instants.insert(TInstant<BaseType>(e));
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

template TSequence<GeomPoint>::TSequence(set<TInstant<GeomPoint>> &instants, bool lower_inc,
                                         bool upper_inc, int srid, Interpolation interpolation);
template TSequence<GeomPoint>::TSequence(set<string> const &instants, bool lower_inc,
                                         bool upper_inc, int srid, Interpolation interpolation);
template TSequence<GeomPoint>::TSequence(string const &serialized, int srid);

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

template TSequence<GeomPoint> TSequence<GeomPoint>::with_srid(int srid) const;

template <typename BaseType>
TSequence<BaseType> TSequence<BaseType>::with_interp(Interpolation interpolation) const {
  if (this->m_interpolation == interpolation) return *this;
  TSequence<BaseType> sequence = *this;
  sequence.m_interpolation = interpolation;
  return sequence;
}

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

template <typename BaseType>
int TSequence<BaseType>::compare_internal(Temporal<BaseType> const &other) const {
  if (this->duration() != other.duration()) {
    throw std::invalid_argument("Unsupported types for comparision");
  }

  TSequence<BaseType> const *that = dynamic_cast<TSequence<BaseType> const *>(&other);

  // Compare instant by instant
  auto lhs_instants = this->instants();
  auto rhs_instants = that->instants();
  auto lhs = lhs_instants.begin();
  auto rhs = rhs_instants.begin();
  while (lhs != lhs_instants.end() && rhs != rhs_instants.end()) {
    if (*lhs < *rhs) return -1;
    if (*lhs > *rhs) return 1;
    lhs++;
    rhs++;
  }

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

template <> int TSequence<GeomPoint>::compare(Temporal<GeomPoint> const &other) const {
  // Compare instants and bounds
  int cmp = compare_internal(other);
  if (cmp != 0) {
    return cmp;
  }

  TSequence<GeomPoint> const *that = dynamic_cast<TSequence<GeomPoint> const *>(&other);

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
TSequence<BaseType> TSequence<BaseType>::atPeriod(Period const &period) const {
  Period inter = this->period().intersection(period);
  auto interp = this->interpolation();

  /* Intersecting period is instantaneous */
  if (inter.lower() == inter.upper()) {
    set<TInstant<BaseType>> instants;
    TInstant<BaseType> instant = this->atTimestamp(inter.lower());
    instants.insert(instant);
    return TSequence<BaseType>(instants, true, true, interp);
  }

  int n = this->findTimestamp(inter.lower());

  /* If the lower bound of the intersecting period is exclusive */
  if (n == -1) n = 0;
  set<TInstant<BaseType>> instants;

  /* Compute the value at the beginning of the intersecting period */
  TInstant<BaseType> inst1 = this->instantN(n);
  TInstant<BaseType> inst2 = this->instantN(n + 1);
  instants.insert(at_timestamp1(inst1, inst2, interp, inter.lower()));
  for (size_t i = n + 2; i < this->m_instants.size(); i++) {
    /* If the end of the intersecting period is between inst1 and inst2 */
    if (inst1.getTimestamp() <= inter.upper() && inter.upper() <= inst2.getTimestamp()) break;

    inst1 = inst2;
    inst2 = this->instantN(i);
    /* If the intersecting period contains inst1 */
    if (inter.lower() <= inst1.getTimestamp() && inst1.getTimestamp() <= inter.upper())
      instants.insert(inst1);
  }

  /* The last two values of sequences with step interpolation and
   * exclusive upper bound must be equal */
  if (interp == Interpolation::Linear || inter.upper_inc())
    instants.insert(at_timestamp1(inst1, inst2, interp, inter.upper()));
  else {
    auto value = instants.rbegin()->getValue();
    instants.insert(TInstant<BaseType>(value, inter.upper()));
  }

  /* Since by definition the sequence is normalized it is not necessary to
   * normalize the projection of the sequence to the period */
  TSequence<BaseType> result(instants, inter.lower_inc(), inter.upper_inc(), interp);
  return result;
}

template <typename BaseType> int TSequence<BaseType>::findTimestamp(time_point t) const {
  int first = 0;
  int const seq_size = this->m_instants.size();
  int last = seq_size - 2;
  int middle = (first + last) / 2;
  while (first <= last) {
    TInstant<BaseType> inst1 = this->instantN(middle);
    TInstant<BaseType> inst2 = this->instantN(middle + 1);
    bool lower_inc = (middle == 0) ? this->period().lower_inc() : true;
    bool upper_inc = (middle == seq_size - 2) ? this->period().upper_inc() : false;
    if ((inst1.getTimestamp() < t && t < inst2.getTimestamp())
        || (lower_inc && inst1.getTimestamp() == t) || (upper_inc && inst2.getTimestamp() == t))
      return middle;
    if (t <= inst1.getTimestamp())
      last = middle - 1;
    else
      first = middle + 1;
    middle = (first + last) / 2;
  }
  return -1;
}

/**
 * Returns a point interpolated from the geometry/geography segment with
 * respect to the fraction of its total length.
 *
 * @param[in] start,end Points defining the segment
 * @param[in] ratio Float between 0 and 1 representing the fraction of the
 * total length of the segment where the point must be located
 */
GeomPoint geoseg_interpolate_point(GeomPoint start, GeomPoint end, double ratio) {
  int srid = start.srid();
  // TODO add support for geodetic
  // bool geodetic = start.geodetic();
  GeomPoint p(start.x() + ((end.x() - start.x()) * ratio),
              start.y() + ((end.y() - start.y()) * ratio), srid);
  return p;
}

template <typename BaseType> BaseType value_at_timestamp1(TInstant<BaseType> const & /*inst1*/,
                                                          TInstant<BaseType> const & /*inst2*/,
                                                          Interpolation /*interpolation*/,
                                                          time_point /*t*/) {
  // Check template specializations for double and GeomPoint
  throw invalid_argument("Unsupported base type for function");
}

template <> float value_at_timestamp1(TInstant<float> const &inst1, TInstant<float> const &inst2,
                                      Interpolation interpolation, time_point t) {
  auto start = inst1.getValue();
  auto end = inst2.getValue();
  /* Constant segment or t is equal to lower bound or step interpolation */
  if (start == end || inst1.getTimestamp() == t
      || (interpolation != Interpolation::Linear && t < inst2.getTimestamp()))
    return start;

  /* t is equal to upper bound */
  if (inst2.getTimestamp() == t) return end;

  /* Interpolation for types with linear interpolation */
  auto duration1 = t - inst1.getTimestamp();
  auto duration2 = inst2.getTimestamp() - inst1.getTimestamp();
  float ratio = ((float)duration1.count()) / duration2.count();
  return start + (end - start) * ratio;
}

template <> GeomPoint value_at_timestamp1(TInstant<GeomPoint> const &inst1,
                                          TInstant<GeomPoint> const &inst2,
                                          Interpolation interpolation, time_point t) {
  auto start = inst1.getValue();
  auto end = inst2.getValue();
  /* Constant segment or t is equal to lower bound or step interpolation */
  if (start == end || inst1.getTimestamp() == t
      || (interpolation != Interpolation::Linear && t < inst2.getTimestamp()))
    return start;

  /* t is equal to upper bound */
  if (inst2.getTimestamp() == t) return end;

  /* Interpolation for types with linear interpolation */
  auto duration1 = t - inst1.getTimestamp();
  auto duration2 = inst2.getTimestamp() - inst1.getTimestamp();
  double ratio = duration1 / duration2;
  return geoseg_interpolate_point(start, end, ratio);
}

template <typename BaseType>
TInstant<BaseType> at_timestamp1(TInstant<BaseType> const &inst1, TInstant<BaseType> const &inst2,
                                 Interpolation interpolation, time_point t) {
  auto value = value_at_timestamp1(inst1, inst2, interpolation, t);
  return TInstant<BaseType>(value, t);
}

template <typename BaseType>
TInstant<BaseType> TSequence<BaseType>::atTimestamp(time_point t) const {
  /* Bounding box test */
  if (!this->period().contains_timestamp(t))
    throw invalid_argument("Specified timestamp does not overlap with this sequence");

  /* Instantaneous sequence */
  if (this->m_instants.size() == 1) return this->startInstant();

  /* General case */
  int n = this->findTimestamp(t);
  TInstant<BaseType> inst1 = this->instantN(n);
  TInstant<BaseType> inst2 = this->instantN(n + 1);
  return at_timestamp1(inst1, inst2, this->interpolation(), t);
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
  validate();
  return in;
}

template <> istream &TSequence<GeomPoint>::read(istream &in, bool with_interp, bool with_srid) {
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
  validate();
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
ostream &TSequence<GeomPoint>::write(ostream &os, bool with_interp, bool with_srid) const {
  if (with_srid && this->srid() != 0) {
    os << "SRID=" << this->srid() << ";";
  }

  write_internal(os, with_interp);
  return os;
}

template class TSequence<bool>;
template class TSequence<int>;
template class TSequence<float>;
template class TSequence<string>;
template class TSequence<GeomPoint>;

}  // namespace meos
