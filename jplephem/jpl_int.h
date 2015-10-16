#ifndef _JPL_INT_H
#define _JPL_INT_H

/* Define a new "long" which is 4 bytes in size -- Addition by George Hobbs */
//typedef unsigned int JPLlong;
typedef int JPLlong;

/* Right now,  DEs 403 and 405 have the maximum kernel size,  of 2036.    */
/* This value may need to be updated the next time JPL releases a new DE: */

#define MAX_KERNEL_SIZE 2036

/***** THERE IS NO NEED TO MODIFY THE REST OF THIS SOURCE (I hope) *********/


/* A JPL binary ephemeris header contains five doubles and */
/* (up to) 41 long integers,  so:                          */


#define JPL_HEADER_SIZE (5 * sizeof( double) + 41 * sizeof( JPLlong))


#pragma pack(1)

struct jpl_eph_data {
    double ephem_start, ephem_end, ephem_step;
    JPLlong ncon;
    double au;
    double emrat;
    JPLlong ipt[13][3];
    JPLlong ephemeris_version;
    JPLlong kernel_size, recsize, ncoeff;
    JPLlong swap_bytes;
    JPLlong curr_cache_loc;
    double pvsun[6];
    double *cache;
    void *iinfo;
    FILE *ifile;
};

struct interpolation_info
{
    double pc[18],vc[18], twot;
    int np, nv;
};

#pragma pack()

#endif
