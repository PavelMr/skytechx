/***************************************************************************
 *******                  JPLEPH.H                                  *********
 ****************************************************************************
 **  This header file is used both by ASC2EPH and TESTEPH programs.        **
 ****************************************************************************
 **  Written: May 28, 1997 by PAD   **  Last modified: June 23,1997 by PAD **
 **  Modified further by Bill Gray,  Jun-Aug 2001                          **
 ****************************************************************************
 **  PAD: dr. Piotr A. Dybczynski,          e-mail: dybol@phys.amu.edu.pl  **
 **   Astronomical Observatory of the A.Mickiewicz Univ., Poznan, Poland   **
 ***************************************************************************/

#ifndef _JPL_EPH_H
#define _JPL_EPH_H

    void * jpl_init_ephemeris( const char *ephemeris_filename,
            char nam[][6], double *val);
    void jpl_close_ephemeris( void *ephem);
    int jpl_state( void *ephem, const double et[2], const int list[12],
            double pv[][6], double nut[4], const int bary);
    int jpl_pleph( void *ephem, const double et[2], const int ntarg,
            const int ncent, double rrd[], const int calc_velocity);
    double jpl_get_double( const void *ephem, const int value);
    double jpl_get_long( const void *ephem, const int value);
    int  make_sub_ephem( const void *ephem, const char *sub_filename,
            const double start_jd, const double end_jd);


/* Following are constants used in          */
/* jpl_get_double( ) and jpl_get_long( ):   */

#define JPL_EPHEM_START_JD               0
#define JPL_EPHEM_END_JD                 8
#define JPL_EPHEM_STEP                  16
#define JPL_EPHEM_N_CONSTANTS           24
#define JPL_EPHEM_AU_IN_KM              28
#define JPL_EPHEM_EARTH_MOON_RATIO      36
#define JPL_EPHEM_EPHEMERIS_VERSION    200
#define JPL_EPHEM_KERNEL_SIZE          204
#define JPL_EPHEM_KERNEL_RECORD_SIZE   208
#define JPL_EPHEM_KERNEL_NCOEFF        212
#define JPL_EPHEM_KERNEL_SWAP_BYTES    216

#endif

