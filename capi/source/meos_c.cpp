#include <meos/meos_c.h>

#include <cstring>
#include <meos/types/box/TBox.hpp>
#include <meos/types/temporal/TSequenceSet.hpp>
#include <meos/types/time/Period.hpp>
#include <sstream>

extern "C" {

// TBox

MEOS_TBox *MEOS_newTBox(double const xmin, double const xmax) {
  return reinterpret_cast<MEOS_TBox *>(new meos::TBox(xmin, xmax));
}

double MEOS_TBox_xmin(MEOS_TBox *tbox) {
  auto t = reinterpret_cast<meos::TBox *>(tbox);
  return t->xmin();
}

double MEOS_TBox_xmax(MEOS_TBox *tbox) {
  auto t = reinterpret_cast<meos::TBox *>(tbox);
  return t->xmax();
}

void MEOS_deleteTBox(MEOS_TBox *tbox) {
  auto t = reinterpret_cast<meos::TBox *>(tbox);
  delete t;
}

// Period

MEOS_Period *MEOS_newPeriod(char *serialized) {
  return reinterpret_cast<MEOS_Period *>(new meos::Period(serialized));
}

// Remember to free the result!
unsigned char *MEOS_Period_str(MEOS_Period *period, size_t *size) {
  auto t = reinterpret_cast<meos::Period *>(period);
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

void MEOS_deletePeriod(MEOS_Period *period) {
  auto t = reinterpret_cast<meos::Period *>(period);
  delete t;
}

// TFloatSeqSet

MEOS_TFloatSeqSet *MEOS_newTFloatSeqSet(char *serialized) {
  return reinterpret_cast<MEOS_TFloatSeqSet *>(new meos::TFloatSeqSet(serialized));
}

MEOS_TFloatSeqSet *MEOS_TFloatSeqSet_atPeriod(MEOS_TFloatSeqSet *tfloatseqset,
                                              MEOS_Period *period) {
  auto t = reinterpret_cast<meos::TFloatSeqSet *>(tfloatseqset);
  auto p = reinterpret_cast<meos::Period *>(period);
  auto r = t->atPeriod(*p);
  return reinterpret_cast<MEOS_TFloatSeqSet *>(new meos::TFloatSeqSet(r));
}

// Remember to free the result!
unsigned char *MEOS_TFloatSeqSet_str(MEOS_TFloatSeqSet *tfloatseqset, size_t *size) {
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

void MEOS_deleteTFloatSeqSet(MEOS_TFloatSeqSet *tfloatseqset) {
  auto t = reinterpret_cast<meos::TFloatSeqSet *>(tfloatseqset);
  delete t;
}
}
