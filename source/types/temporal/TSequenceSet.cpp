#include <iomanip>
#include <meos/types/temporal/TSequenceSet.hpp>

template <typename T> TSequenceSet<T>::TSequenceSet() {}

template <typename T>
TSequenceSet<T>::TSequenceSet(set<TSequence<T>> const &sequences)
    : m_sequences(sequences) {}

template <typename T> TSequenceSet<T>::TSequenceSet(set<string> const &ss) {
  for (auto const &e : ss)
    this->m_sequences.insert(TSequence<T>(e));
}

template <typename T> TSequenceSet<T>::TSequenceSet(string const &serialized) {
  stringstream ss(serialized);
  TSequenceSet<T> sequence_set;
  ss >> sequence_set;
  this->m_sequences = sequence_set.sequences();
}

template <typename T>
int TSequenceSet<T>::compare(Temporal<T> const &other) const {
  if (this->duration() != other.duration()) {
    throw std::invalid_argument("Unsupported types for comparision");
  }

  TSequenceSet<T> const *that = dynamic_cast<TSequenceSet<T> const *>(&other);
  // Compare number of sequences
  if (this->m_sequences.size() < that->m_sequences.size())
    return -1;
  if (this->m_sequences.size() > that->m_sequences.size())
    return 1;

  // Compare sequence by sequence
  auto lhs_sequences = this->sequences();
  auto rhs_sequences = that->sequences();
  auto lhs = lhs_sequences.begin();
  auto rhs = rhs_sequences.begin();
  while (lhs != lhs_sequences.end()) {
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

template <typename T> set<TSequence<T>> TSequenceSet<T>::sequences() const {
  return this->m_sequences;
}

/**
 * Number of distinct sequences.
 */
template <typename T> size_t TSequenceSet<T>::numSequences() const {
  return this->m_sequences.size();
}

/**
 * Start sequence.
 */
template <typename T> TSequence<T> TSequenceSet<T>::startSequence() const {
  auto s = this->m_sequences;
  if (s.size() <= 0) {
    throw "At least one sequence expected";
  }
  return *s.begin();
}

/**
 * End sequence.
 */
template <typename T> TSequence<T> TSequenceSet<T>::endSequence() const {
  auto s = this->m_sequences;
  if (s.size() <= 0) {
    throw "At least one sequence expected";
  }
  return *s.rbegin();
}

/**
 * N-th distinct sequence.
 */
template <typename T> TSequence<T> TSequenceSet<T>::sequenceN(size_t n) const {
  auto s = this->m_sequences;
  if (s.size() <= n) {
    throw "At least " + std::to_string(n) + " sequence(s) expected";
  }
  return *next(s.begin(), n);
}

template <typename T> set<TInstant<T>> TSequenceSet<T>::instants() const {
  set<TInstant<T>> s;
  for (auto const &e : this->m_sequences) {
    for (auto const &e : e.instants()) {
      s.insert(e);
    }
  }
  return s;
}

template <typename T> duration_ms TSequenceSet<T>::timespan() const {
  duration_ms tspan(0);
  for (auto const &e : this->m_sequences)
    tspan += e.timespan();
  return tspan;
}

template <typename T> set<Range<T>> TSequenceSet<T>::getValues() const {
  set<Range<T>> s;
  for (auto const &e : this->m_sequences) {
    for (auto const &e : e.getValues()) {
      s.insert(e);
    }
  }
  return s;
}

template <typename T> set<time_point> TSequenceSet<T>::timestamps() const {
  set<time_point> s;
  for (auto const &e : this->m_sequences) {
    for (auto const &e : e.timestamps()) {
      s.insert(e);
    }
  }
  return s;
}

template <typename T> PeriodSet TSequenceSet<T>::getTime() const {
  set<Period> s;
  for (auto const &e : sequences()) {
    s.insert(e.period());
  }
  return PeriodSet(s);
}

template <typename T> Period TSequenceSet<T>::period() const {
  return Period(this->startTimestamp(), this->endTimestamp(), true, true);
}

template <typename T>
unique_ptr<TSequenceSet<T>>
TSequenceSet<T>::shift(duration_ms const timedelta) const {
  return unique_ptr<TSequenceSet<T>>(this->shift_impl(timedelta));
}

template <typename T>
TSequenceSet<T> *
TSequenceSet<T>::shift_impl(duration_ms const timedelta) const {
  set<TSequence<T>> s;
  for (auto const &e : this->m_sequences) {
    s.insert(*e.shift(timedelta).get());
  }
  return new TSequenceSet<T>(s);
}

template <typename T>
bool TSequenceSet<T>::intersectsTimestamp(time_point const datetime) const {
  for (auto const &t : this->timestamps()) {
    if (t == datetime) {
      return true;
    }
  }
  return false;
}

template <typename T>
bool TSequenceSet<T>::intersectsPeriod(Period const period) const {
  for (auto const &t : this->timestamps()) {
    if (period.contains_timestamp(t)) {
      return true;
    }
  }
  return false;
}
