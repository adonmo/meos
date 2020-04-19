#ifndef MEOS_TO_MWKTREADER_HPP
#define MEOS_TO_MWKTREADER_HPP

#include <iomanip>
#include <meos/types/temporal/TInstant.hpp>
#include <meos/types/temporal/TInstantSet.hpp>
#include <meos/types/temporal/TSequence.hpp>
#include <meos/types/temporal/Temporal.hpp>
#include <string>

using namespace std;

template <typename T> class Parser {
public:
  Parser(const string &in);
  unique_ptr<Temporal<T>> parseNextTemporal();
  unique_ptr<TSequence<T>> parseNextTSequence();
  unique_ptr<TInstantSet<T>> parseNextTInstantSet();
  unique_ptr<TInstant<T>> parseNextTInstant();

  /**
   * Parse time in ISO8601 format
   */
  time_t parseNextTime();
  T parseNextValue();

private:
  const string in;
  string::const_iterator iter;
  char peek(int lookahead);
  void skipWhitespaces();
  void consumeChar(const char c);
  void skipChars(const string &chars);
  bool hasNext();
};

template class Parser<int>;
template class Parser<float>;

#endif
