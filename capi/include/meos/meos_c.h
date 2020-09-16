#pragma once

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MEOS_TBox MEOS_TBox;
typedef struct MEOS_Period MEOS_Period;
typedef struct MEOS_TFloatSeqSet MEOS_TFloatSeqSet;

// TBox

MEOS_TBox *MEOS_newTBox(double const xmin, double const xmax);

double MEOS_TBox_xmin(MEOS_TBox *tbox);
double MEOS_TBox_xmax(MEOS_TBox *tbox);

void MEOS_deleteTBox(MEOS_TBox *tbox);

// Period

MEOS_Period *MEOS_newPeriod(char *serialized);

void MEOS_deletePeriod(MEOS_Period *period);

// TFloatSeqSet

MEOS_TFloatSeqSet *MEOS_newTFloatSeqSet(char *serialized);

MEOS_TFloatSeqSet *MEOS_TFloatSeqSet_atPeriod(MEOS_TFloatSeqSet *tfloatseqset, MEOS_Period *period);
unsigned char *MEOS_TFloatSeqSet_str(MEOS_TFloatSeqSet *tfloatseqset, size_t *size);

void MEOS_deleteTFloatSeqSet(MEOS_TFloatSeqSet *tfloatseqset);

#ifdef __cplusplus
}
#endif
