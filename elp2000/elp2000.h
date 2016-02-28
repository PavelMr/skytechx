#ifndef ELP2000_H
#define ELP2000_H


class ELP2000
{
public:
  ELP2000();

  double EclipticLongitude(double JD);
  double EclipticLatitude(double JD);
  double RadiusVector(double JD);

  void EclipticRectangularCoordinates(double JD, double *data);

  void solve(double jd, double *data);
};

#endif // ELP2000_H
