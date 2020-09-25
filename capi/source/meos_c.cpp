#include <meos/meos_c.h>

#include <cstring>
#include <ctime>
#include <meos/types/box/TBox.hpp>
#include <meos/types/temporal/TSequence.hpp>
#include <meos/types/temporal/TSequenceSet.hpp>
#include <meos/types/time/Period.hpp>
#include <sstream>

extern "C" {

/*****************************************************************************
 * TBox
 *****************************************************************************/

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

/*****************************************************************************
 * Period
 *****************************************************************************/

MEOS_Period *MEOS_newPeriod(char *serialized) {
  return reinterpret_cast<MEOS_Period *>(new meos::Period(serialized));
}

MEOS_Period *MEOS_newPeriod_TT(time_t lower, time_t upper) {
  return reinterpret_cast<MEOS_Period *>(
      new meos::Period(std::chrono::system_clock::from_time_t(lower),
                       std::chrono::system_clock::from_time_t(upper)));
}

MEOS_Period *MEOS_newPeriod_TTBB(time_t lower, time_t upper, bool lower_inc, bool upper_inc) {
  return reinterpret_cast<MEOS_Period *>(
      new meos::Period(std::chrono::system_clock::from_time_t(lower),
                       std::chrono::system_clock::from_time_t(upper), lower_inc, upper_inc));
}

// Remember to free the result!
char *MEOS_Period_str(MEOS_Period *period) {
  auto t = reinterpret_cast<meos::Period *>(period);
  std::stringstream output;
  output << *t;
  auto s = output.str();
  const std::size_t len = s.length();
  char *result = new char[len + 1];
  if (result) {
    std::strcpy(result, s.c_str());
  }
  return result;
}

void MEOS_deletePeriod(MEOS_Period *period) {
  auto t = reinterpret_cast<meos::Period *>(period);
  delete t;
}

/*****************************************************************************
 * TFloatInst
 *****************************************************************************/

MEOS_TFloatInst *MEOS_newTFloatInst(char *serialized) {
  return reinterpret_cast<MEOS_TFloatInst *>(new meos::TFloatInst(serialized));
}

MEOS_TFloatInst *MEOS_newTFloatInst_VT(float value, time_t timestamp) {
  return reinterpret_cast<MEOS_TFloatInst *>(
      new meos::TFloatInst(value, std::chrono::system_clock::from_time_t(timestamp)));
}

// Remember to free the result!
char *MEOS_TFloatInst_str(MEOS_TFloatInst *tfloatinst) {
  auto t = reinterpret_cast<meos::TFloatInst *>(tfloatinst);
  std::stringstream output;
  output << *t;
  auto s = output.str();
  const std::size_t len = s.length();
  char *result = new char[len + 1];
  if (result) {
    std::strcpy(result, s.c_str());
  }
  return result;
}

void MEOS_deleteTFloatInst(MEOS_TFloatInst *tfloatinst) {
  auto t = reinterpret_cast<meos::TFloatInst *>(tfloatinst);
  delete t;
}

/*****************************************************************************
 * TFloatSeq
 *****************************************************************************/

MEOS_TFloatSeq *MEOS_newTFloatSeq(char *serialized) {
  return reinterpret_cast<MEOS_TFloatSeq *>(new meos::TFloatSeq(serialized));
}

MEOS_TFloatSeq *MEOS_newTFloatSeq_IBBI(MEOS_TFloatInst **instants, int count, bool lower_inc,
                                       bool upper_inc, MEOS_Interpolation interpolation) {
  std::set<meos::TFloatInst> s;
  for (size_t i = 0; i < count; i++) {
    auto inst = reinterpret_cast<meos::TFloatInst *>(instants[i]);
    s.insert(*inst);
  }
  auto interp = interpolation == MEOS_Interpolation_Linear ? meos::Interpolation::Linear
                                                           : meos::Interpolation::Stepwise;
  return reinterpret_cast<MEOS_TFloatSeq *>(new meos::TFloatSeq(s, lower_inc, upper_inc, interp));
}

MEOS_TFloatSeq *MEOS_newTFloatSeq_IsBBI(char **instants, int count, bool lower_inc, bool upper_inc,
                                        MEOS_Interpolation interpolation) {
  std::set<meos::TFloatInst> s;
  for (size_t i = 0; i < count; i++) {
    meos::TFloatInst inst(instants[i]);
    s.insert(inst);
  }
  auto interp = interpolation == MEOS_Interpolation_Linear ? meos::Interpolation::Linear
                                                           : meos::Interpolation::Stepwise;
  return reinterpret_cast<MEOS_TFloatSeq *>(new meos::TFloatSeq(s, lower_inc, upper_inc, interp));
}

bool MEOS_TFloatSeq_lower_inc(MEOS_TFloatSeq *tfloatseq) {
  auto t = reinterpret_cast<meos::TFloatSeq *>(tfloatseq);
  return t->lower_inc();
}

bool MEOS_TFloatSeq_upper_inc(MEOS_TFloatSeq *tfloatseq) {
  auto t = reinterpret_cast<meos::TFloatSeq *>(tfloatseq);
  return t->upper_inc();
}

MEOS_Interpolation MEOS_TFloatSeq_interpolation(MEOS_TFloatSeq *tfloatseq) {
  auto t = reinterpret_cast<meos::TFloatSeq *>(tfloatseq);
  return t->interpolation() == meos::Interpolation::Linear ? MEOS_Interpolation_Linear
                                                           : MEOS_Interpolation_Stepwise;
}

MEOS_TFloatInst **MEOS_TFloatSeq_instants(MEOS_TFloatSeq *tfloatseq, int *count) {
  auto t = reinterpret_cast<meos::TFloatSeq *>(tfloatseq);
  auto r = t->instants();
  *count = r.size();
  MEOS_TFloatInst **instants = new MEOS_TFloatInst *[*count];
  size_t i = 0;
  for (auto const &it : r) {
    auto x = new meos::TFloatInst(it);
    instants[i++] = reinterpret_cast<MEOS_TFloatInst *>(x);
  }
  return instants;
}

// Remember to free the result!
char *MEOS_TFloatSeq_str(MEOS_TFloatSeq *tfloatseq) {
  auto t = reinterpret_cast<meos::TFloatSeq *>(tfloatseq);
  std::stringstream output;
  output << *t;
  auto s = output.str();
  const std::size_t len = s.length();
  char *result = new char[len + 1];
  if (result) {
    std::strcpy(result, s.c_str());
  }
  return result;
}

void MEOS_deleteTFloatSeq(MEOS_TFloatSeq *tfloatseq) {
  auto t = reinterpret_cast<meos::TFloatSeq *>(tfloatseq);
  delete t;
}

/*****************************************************************************
 * TFloatSeqSet
 *****************************************************************************/

MEOS_TFloatSeqSet *MEOS_newTFloatSeqSet(char *serialized) {
  return reinterpret_cast<MEOS_TFloatSeqSet *>(new meos::TFloatSeqSet(serialized));
}

MEOS_TFloatSeqSet *MEOS_newTFloatSeqSet_SI(MEOS_TFloatSeq **sequences, int count,
                                           MEOS_Interpolation interpolation) {
  std::set<meos::TFloatSeq> s;
  for (size_t i = 0; i < count; i++) {
    auto seq = reinterpret_cast<meos::TFloatSeq *>(sequences[i]);
    s.insert(*seq);
  }
  auto interp = interpolation == MEOS_Interpolation_Linear ? meos::Interpolation::Linear
                                                           : meos::Interpolation::Stepwise;
  return reinterpret_cast<MEOS_TFloatSeqSet *>(new meos::TFloatSeqSet(s, interp));
}

MEOS_TFloatSeqSet *MEOS_newTFloatSeqSet_SsI(char **sequences, int count,
                                            MEOS_Interpolation interpolation) {
  std::set<meos::TFloatSeq> s;
  for (size_t i = 0; i < count; i++) {
    meos::TFloatSeq seq(sequences[i]);
    s.insert(seq);
  }
  auto interp = interpolation == MEOS_Interpolation_Linear ? meos::Interpolation::Linear
                                                           : meos::Interpolation::Stepwise;
  return reinterpret_cast<MEOS_TFloatSeqSet *>(new meos::TFloatSeqSet(s, interp));
}

MEOS_TFloatSeq **MEOS_TFloatSeqSet_sequences(MEOS_TFloatSeqSet *tfloatseqset, int *count) {
  auto t = reinterpret_cast<meos::TFloatSeqSet *>(tfloatseqset);
  auto r = t->sequences();
  *count = r.size();
  MEOS_TFloatSeq **sequences = new MEOS_TFloatSeq *[*count];
  size_t i = 0;
  for (auto const &it : r) {
    auto x = new meos::TFloatSeq(it);
    sequences[i++] = reinterpret_cast<MEOS_TFloatSeq *>(x);
  }
  return sequences;
}

MEOS_TFloatSeqSet *MEOS_TFloatSeqSet_atPeriod(MEOS_TFloatSeqSet *tfloatseqset,
                                              MEOS_Period *period) {
  auto t = reinterpret_cast<meos::TFloatSeqSet *>(tfloatseqset);
  auto p = reinterpret_cast<meos::Period *>(period);
  auto r = t->atPeriod(*p);
  return reinterpret_cast<MEOS_TFloatSeqSet *>(new meos::TFloatSeqSet(r));
}

// Remember to free the result!
char *MEOS_TFloatSeqSet_str(MEOS_TFloatSeqSet *tfloatseqset) {
  auto t = reinterpret_cast<meos::TFloatSeqSet *>(tfloatseqset);
  std::stringstream output;
  output << *t;
  auto s = output.str();
  const std::size_t len = s.length();
  char *result = new char[len + 1];
  if (result) {
    std::strcpy(result, s.c_str());
  }
  return result;
}

void MEOS_deleteTFloatSeqSet(MEOS_TFloatSeqSet *tfloatseqset) {
  auto t = reinterpret_cast<meos::TFloatSeqSet *>(tfloatseqset);
  delete t;
}
}
