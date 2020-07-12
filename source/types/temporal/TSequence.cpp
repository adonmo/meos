#include <iomanip>
#include <sstream>
#include <string>

#include <meos/types/temporal/TSequence.hpp>

template <typename T> TSequence<T>::TSequence() {}

template <typename T>
TSequence<T>::TSequence(set<TInstant<T>> &instants, bool lower_inc,
                        bool upper_inc, Interpolation interpolation)
    : m_instants(instants), m_lower_inc(lower_inc), m_upper_inc(upper_inc),
      m_interpolation(interpolation) {
  validate();
}

template <typename T>
TSequence<T>::TSequence(set<string> const &instants, bool lower_inc,
                        bool upper_inc, Interpolation interpolation)
    : m_lower_inc(lower_inc), m_upper_inc(upper_inc),
      m_interpolation(interpolation) {
  TSequence<T> instant_set;
  for (auto const &e : instants)
    m_instants.insert(TInstant<T>(e));
  validate();
}

template <typename T> TSequence<T>::TSequence(string const &serialized) {
  stringstream ss(serialized);
  TSequence<T> seq;
  ss >> seq;
  this->m_instants = seq.instants();
  this->m_lower_inc = seq.lower_inc();
  this->m_upper_inc = seq.upper_inc();
  this->m_interpolation = seq.interpolation();
  validate();
}

template <typename T> void TSequence<T>::validate() const {
  size_t sz = this->m_instants.size();
  if (sz < 1) {
    throw invalid_argument("A sequence should have at least one instant");
  }

  if (this->m_interpolation == Interpolation::Linear && is_discrete_v<T>) {
    throw invalid_argument(
        "Cannot assign linear interpolation to a discrete base type");
  }

  // TODO
  // For stepwise interp and exclusive upper bound, last two values should match

  time_point start = this->startTimestamp();
  time_point end = this->endTimestamp();

  if (start > end) {
    throw invalid_argument(
        "The lower bound must be less than or equal to the upper bound");
  }

  if (start == end) {
    bool both_inclusive = this->lower_inc() && this->upper_inc();
    if (!both_inclusive) {
      throw invalid_argument("The lower and upper bounds must be inclusive for "
                             "an instantaneous sequence");
    }
  }
}

template <typename T>
int TSequence<T>::compare(Temporal<T> const &other) const {
  if (this->duration() != other.duration()) {
    throw std::invalid_argument("Unsupported types for comparision");
  }

  TSequence<T> const *that = dynamic_cast<TSequence<T> const *>(&other);
  // Compare number of instants
  if (this->m_instants.size() < that->m_instants.size())
    return -1;
  if (this->m_instants.size() > that->m_instants.size())
    return 1;

  // Compare bounds
  // [ < (, ) < ]
  if ((this->m_lower_inc && !that->m_lower_inc) ||
      (!this->m_upper_inc && that->m_upper_inc))
    return -1;
  // ( > [, ] > )
  if ((that->m_lower_inc && !this->m_lower_inc) ||
      (!that->m_upper_inc && this->m_upper_inc))
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

template <typename T> bool TSequence<T>::lower_inc() const {
  return this->m_lower_inc;
}
template <typename T> bool TSequence<T>::upper_inc() const {
  return this->m_upper_inc;
}

template <typename T> set<TInstant<T>> TSequence<T>::instants() const {
  return this->m_instants;
}

template <typename T> Interpolation TSequence<T>::interpolation() const {
  return this->m_interpolation;
}

template <typename T> duration_ms TSequence<T>::timespan() const {
  return std::chrono::duration_cast<duration_ms>(this->endTimestamp() -
                                                 this->startTimestamp());
}

template <typename T> set<Range<T>> TSequence<T>::getValues() const {
  if (this->m_instants.size() == 0)
    return {};
  T min = this->m_instants.begin()->getValue();
  T max = this->m_instants.begin()->getValue();
  for (auto const &e : this->m_instants) {
    if (e.getValue() < min) {
      min = e.getValue();
    }
    if (e.getValue() > max) {
      max = e.getValue();
    }
  }
  return {Range<T>(min, max, this->m_lower_inc, this->m_upper_inc)};
}

template <typename T> set<time_point> TSequence<T>::timestamps() const {
  set<time_point> s;
  for (auto const &e : this->m_instants) {
    s.insert(e.getTimestamp());
  }
  return s;
}

template <typename T> PeriodSet TSequence<T>::getTime() const {
  set<Period> s = {this->period()};
  return PeriodSet(s);
}

template <typename T> Period TSequence<T>::period() const {
  return Period(this->startTimestamp(), this->endTimestamp(), m_lower_inc,
                m_upper_inc);
}

template <typename T>
unique_ptr<TSequence<T>>
TSequence<T>::shift(duration_ms const timedelta) const {
  return unique_ptr<TSequence<T>>(this->shift_impl(timedelta));
}

template <typename T>
TSequence<T> *TSequence<T>::shift_impl(duration_ms const timedelta) const {
  set<TInstant<T>> s;
  for (auto const &e : this->m_instants) {
    s.insert(TInstant<T>(e.getValue(), e.getTimestamp() + timedelta));
  }
  return new TSequence<T>(s, m_lower_inc, m_upper_inc);
}

template <typename T>
bool TSequence<T>::intersectsTimestamp(time_point const datetime) const {
  return this->period().contains_timestamp(datetime);
}

template <typename T>
bool TSequence<T>::intersectsPeriod(Period const period) const {
  return this->period().overlap(period);
}

template <typename T>
istream &TSequence<T>::read(istream &in, bool with_interp) {
  char c;

  Interpolation interp =
      is_discrete_v<T> ? Interpolation::Stepwise : Interpolation::Linear;

  if (with_interp) {
    // First we check for interpolation, if specified, else we stick with
    // default value (Stepwise for discrete base types, Linear otherwise)
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

  this->m_instants = s;
  this->m_lower_inc = lower_inc;
  this->m_upper_inc = upper_inc;
  this->m_interpolation = interp;

  return in;
}

template <typename T>
ostream &TSequence<T>::write(ostream &os, bool with_interp) const {
  if (with_interp && this->interpolation() != default_interp_v<T>) {
    os << "Interp=" << this->interpolation() << ";";
  }

  bool first = true;
  os << (this->m_lower_inc ? "[" : "(");
  for (auto const &instant : this->instants()) {
    if (first)
      first = false;
    else
      os << ", ";
    os << instant;
  }
  os << (this->m_upper_inc ? "]" : ")");
  return os;
}
