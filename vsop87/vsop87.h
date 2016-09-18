/***********************************************************************
This file is part of SkytechX.

SkytechX is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SkytechX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SkytechX.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/

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

