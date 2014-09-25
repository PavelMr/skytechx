
#ifndef PLANTBL_H
#define PLANTBL_H

struct plantbl {
  char max_harmonic[9];
  char max_power_of_t;
  char *arg_tbl;
  double *lon_tbl;
  double *lat_tbl;
  double *rad_tbl;
  double distance;
};

void de404(int pln, double jd, double rect[]);
int gmoon404(double J, double rect[], double pol[]);

#endif
