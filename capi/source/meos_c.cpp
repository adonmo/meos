#include <meos/meos_c.h>

#include <meos/types/box/TBox.hpp>

extern "C" {
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
}
