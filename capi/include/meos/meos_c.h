#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TBox TBox;

TBox *newTBox(double const xmin, double const xmax);

double TBox_xmin(TBox *tbox);
double TBox_xmax(TBox *tbox);

void deleteTBox(TBox *v);

#ifdef __cplusplus
}
#endif
