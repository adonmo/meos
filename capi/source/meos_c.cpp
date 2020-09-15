#include <meos/meos_c.h>

#include <cstring>
#include <meos/types/box/TBox.hpp>
#include <meos/types/temporal/TSequenceSet.hpp>
#include <meos/types/time/Period.hpp>
#include <sstream>

extern "C" {

// TBox

TBox *newTBox(double const xmin, double const xmax) {
  return reinterpret_cast<TBox *>(new meos::TBox(xmin, xmax));
}

double TBox_xmin(TBox *tbox) {
  auto t = reinterpret_cast<meos::TBox *>(tbox);
  return t->xmin();
}

double TBox_xmax(TBox *tbox) {
  auto t = reinterpret_cast<meos::TBox *>(tbox);
  return t->xmax();
}

void deleteTBox(TBox *tbox) {
  auto t = reinterpret_cast<meos::TBox *>(tbox);
  delete t;
}

// Period

Period *newPeriod(char *serialized) {
  return reinterpret_cast<Period *>(new meos::Period(serialized));
}

void deletePeriod(Period *period) {
  auto t = reinterpret_cast<meos::Period *>(period);
  delete t;
}

// TFloatSeqSet

TFloatSeqSet *newTFloatSeqSet(char *serialized) {
  return reinterpret_cast<TFloatSeqSet *>(new meos::TFloatSeqSet(serialized));
}

TFloatSeqSet *TFloatSeqSet_atPeriod(TFloatSeqSet *tfloatseqset, Period *period) {
  auto t = reinterpret_cast<meos::TFloatSeqSet *>(tfloatseqset);
  auto p = reinterpret_cast<meos::Period *>(period);
  auto r = t->atPeriod(*p);
  return reinterpret_cast<TFloatSeqSet *>(new meos::TFloatSeqSet(r));
}

// Remember to free the result!
unsigned char *TFloatSeqSet_str(TFloatSeqSet *tfloatseqset, size_t *size) {
  auto t = reinterpret_cast<meos::TFloatSeqSet *>(tfloatseqset);
  std::stringstream output;
  output << *t;
  auto s = output.str();
  const std::size_t len = s.length();
  unsigned char *result = static_cast<unsigned char *>(malloc(len));
  if (result) {
    std::memcpy(result, s.c_str(), len);
    *size = len;
  }
  return result;
}

void deleteTFloatSeqSet(TFloatSeqSet *tfloatseqset) {
  auto t = reinterpret_cast<meos::TFloatSeqSet *>(tfloatseqset);
  delete t;
}
}
