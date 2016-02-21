#ifndef VSOP87
#define VSOP87

void vsop87(int planet, double jd, double *polar);

void mercury_VSOP87(double jd, double *polar);
void venus_VSOP87(double jd, double *polar);
void earth_VSOP87(double jd, double *polar);
void mars_VSOP87(double jd, double *polar);
void jupiter_VSOP87(double jd, double *polar);
void saturn_VSOP87(double jd, double *polar);
void uranus_VSOP87(double jd, double *polar);
void neptune_VSOP87(double jd, double *polar);


#endif // VSOP87

