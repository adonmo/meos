#pragma once

#include <string.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MEOS_Period MEOS_Period;
typedef struct MEOS_TBox MEOS_TBox;
typedef struct MEOS_TFloatInst MEOS_TFloatInst;
typedef struct MEOS_TFloatSeq MEOS_TFloatSeq;
typedef struct MEOS_TFloatSeqSet MEOS_TFloatSeqSet;

typedef enum {
  MEOS_Interpolation_Stepwise,
  MEOS_Interpolation_Linear,
} MEOS_Interpolation;

/*****************************************************************************
 * TBox
 *****************************************************************************/

MEOS_TBox *MEOS_newTBox(double const xmin, double const xmax);

double MEOS_TBox_xmin(MEOS_TBox *tbox);
double MEOS_TBox_xmax(MEOS_TBox *tbox);

void MEOS_deleteTBox(MEOS_TBox *tbox);

/*****************************************************************************
 * Period
 *****************************************************************************/

// Constructor with a string
MEOS_Period *MEOS_newPeriod(char *serialized);
// Constructor with two timestamps
MEOS_Period *MEOS_newPeriod_TT(time_t lower, time_t upper);
// Constructor with two timestamps and two boolean bounds
MEOS_Period *MEOS_newPeriod_TTBB(time_t lower, time_t upper, bool lower_inc, bool upper_inc);

char *MEOS_Period_str(MEOS_Period *period);

void MEOS_deletePeriod(MEOS_Period *period);

/*****************************************************************************
 * TFloatInst
 *****************************************************************************/

// Constructor with a string
MEOS_TFloatInst *MEOS_newTFloatInst(char *serialized);
// Constructor with a value and timestamp
MEOS_TFloatInst *MEOS_newTFloatInst_VT(float value, time_t timestamp);

char *MEOS_TFloatInst_str(MEOS_TFloatInst *tfloatinst);

void MEOS_deleteTFloatInst(MEOS_TFloatInst *tfloatinst);

/*****************************************************************************
 * TFloatSeq
 *****************************************************************************/

// Constructor with a string
MEOS_TFloatSeq *MEOS_newTFloatSeq(char *serialized);
// Constructor with a set of instants, two boolean bounds and interpolation
MEOS_TFloatSeq *MEOS_newTFloatSeq_IBBI(MEOS_TFloatInst **instants, int count, bool lower_inc,
                                       bool upper_inc, MEOS_Interpolation interpolation);
// Constructor with a set of instants as strings, two boolean bounds and interpolation
MEOS_TFloatSeq *MEOS_newTFloatSeq_IsBBI(char **instants, int count, bool lower_inc, bool upper_inc,
                                        MEOS_Interpolation interpolation);

bool MEOS_TFloatSeq_lower_inc(MEOS_TFloatSeq *tfloatseq);
bool MEOS_TFloatSeq_upper_inc(MEOS_TFloatSeq *tfloatseq);
MEOS_Interpolation MEOS_TFloatSeq_interpolation(MEOS_TFloatSeq *tfloatseq);
MEOS_TFloatInst **MEOS_TFloatSeq_instants(MEOS_TFloatSeq *tfloatseq, int *count);
char *MEOS_TFloatSeq_str(MEOS_TFloatSeq *tfloatseq);

void MEOS_deleteTFloatSeq(MEOS_TFloatSeq *tfloatseq);

/*****************************************************************************
 * TFloatSeqSet
 *****************************************************************************/

// Constructor with a string
MEOS_TFloatSeqSet *MEOS_newTFloatSeqSet(char *serialized);
// Constructor with a set of sequences and interpolation
MEOS_TFloatSeqSet *MEOS_newTFloatSeqSet_SI(MEOS_TFloatSeq **sequences, int count,
                                           MEOS_Interpolation interpolation);
// Constructor with a set of sequences as strings and interpolation
MEOS_TFloatSeqSet *MEOS_newTFloatSeqSet_SsI(char **sequences, int count,
                                            MEOS_Interpolation interpolation);

MEOS_TFloatSeq **MEOS_TFloatSeqSet_sequences(MEOS_TFloatSeqSet *tfloatseqset, int *count);
MEOS_TFloatSeqSet *MEOS_TFloatSeqSet_atPeriod(MEOS_TFloatSeqSet *tfloatseqset, MEOS_Period *period);
char *MEOS_TFloatSeqSet_str(MEOS_TFloatSeqSet *tfloatseqset);

void MEOS_deleteTFloatSeqSet(MEOS_TFloatSeqSet *tfloatseqset);

#ifdef __cplusplus
}
#endif
