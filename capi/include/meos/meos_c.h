#pragma once

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TBox TBox;
typedef struct Period Period;
typedef struct TFloatSeqSet TFloatSeqSet;

// TBox

TBox *newTBox(double const xmin, double const xmax);

double TBox_xmin(TBox *tbox);
double TBox_xmax(TBox *tbox);

void deleteTBox(TBox *tbox);

// Period

Period *newPeriod(char *serialized);

void deletePeriod(Period *period);

// TFloatSeqSet

TFloatSeqSet *newTFloatSeqSet(char *serialized);

TFloatSeqSet *TFloatSeqSet_atPeriod(TFloatSeqSet *tfloatseqset, Period *period);
unsigned char *TFloatSeqSet_str(TFloatSeqSet *tfloatseqset, size_t *size);

void deleteTFloatSeqSet(TFloatSeqSet *tfloatseqset);

#ifdef __cplusplus
}
#endif
