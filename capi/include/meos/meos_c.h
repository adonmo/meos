#pragma once

#include <string.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MEOS_Period MEOS_Period;
typedef struct MEOS_TBox MEOS_TBox;
typedef struct MEOS_TFloatSeq MEOS_TFloatSeq;
typedef struct MEOS_TFloatSeqSet MEOS_TFloatSeqSet;

enum MEOS_Interpolation {
  MEOS_Interpolation_Stepwise,
  MEOS_Interpolation_Linear,
};

// TBox

MEOS_TBox *MEOS_newTBox(double const xmin, double const xmax);

double MEOS_TBox_xmin(MEOS_TBox *tbox);
double MEOS_TBox_xmax(MEOS_TBox *tbox);

void MEOS_deleteTBox(MEOS_TBox *tbox);

// Period

// Constructor with a string
MEOS_Period *MEOS_newPeriod(char *serialized);
// Constructor with two timestamps
MEOS_Period *MEOS_newPeriod_TT(time_t lower, time_t upper);
// Constructor with two timestamps and two boolean bounds
MEOS_Period *MEOS_newPeriod_TTBB(time_t lower, time_t upper, bool lower_inc, bool upper_inc);

char *MEOS_Period_str(MEOS_Period *period);

void MEOS_deletePeriod(MEOS_Period *period);

// TFloatSeqSet

// Constructor with a string
MEOS_TFloatSeqSet *MEOS_newTFloatSeqSet(char *serialized);
// Constructor with a set of sequences and interpolation
MEOS_TFloatSeqSet *MEOS_newTFloatSeqSet_SI(MEOS_TFloatSeq **sequences, int count,
                                           MEOS_Interpolation interpolation);
// Constructor with a set of sequences as strings and interpolation
MEOS_TFloatSeqSet *MEOS_newTFloatSeqSet_SsI(char **sequences, int count,
                                            MEOS_Interpolation interpolation);

MEOS_TFloatSeqSet *MEOS_TFloatSeqSet_atPeriod(MEOS_TFloatSeqSet *tfloatseqset, MEOS_Period *period);
char *MEOS_TFloatSeqSet_str(MEOS_TFloatSeqSet *tfloatseqset);

void MEOS_deleteTFloatSeqSet(MEOS_TFloatSeqSet *tfloatseqset);

#ifdef __cplusplus
}
#endif
