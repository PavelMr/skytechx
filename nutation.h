#ifndef NUTATION_H
#define NUTATION_H

double nutationInLongitude(double JD);
double nutationInObliquity(double JD);

double nutationInRightAscension(double Alpha, double Delta, double Obliquity, double NutationInLongitude, double NutationInObliquity);
double nutationInDeclination(double Alpha, double Obliquity, double NutationInLongitude, double NutationInObliquity);

#endif // NUTATION_H

