#ifndef DSO_DEF_H
#define DSO_DEF_H

#include "skcore.h"

#define NO_DSO_MAG           0x7fff
#define NO_DSO_PA            0xffff
#define NO_DSO_SHAPE         0xffff

#define DSOT_UNKNOWN          0

#define DSOT_GALAXY           1
#define DSOT_GALAXY_CLD       2

#define DSOT_NEBULA           3  // nebula (dark/bright/emmision etc.)
#define DSOT_DARK_NEB         4
#define DSOT_BRIGHT_NEB       5

#define DSOT_PLN_NEBULA       6

#define DSOT_SUPER_REM        7 // supernova remnant

#define DSOT_OPEN_CLUSTER    10
#define DSOT_OPEN_CLS_NEB    11  // open cluster with nebulosity

#define DSOT_GLOB_CLUSTER    12

#define DSOT_STAR            13
#define DSOT_STARS           14
#define DSOT_ASTERISM        15
#define DSOT_QUASAR          16

#define DSOT_MILKY_SC        17
#define DSOT_GAL_CLUSTER     18

#define DSOT_NGC_DUPP            20  // NGC/IC duplicate

#define DSOT_OTHER           30
#define DSOT_COUNT           31

#define DSO_MAG              mag / 100.0

typedef struct
{
  char     id[4];          // DSO2
  qint32   numDso;         // dso count
  qint32   textSegSize;    // byte size
  qint32   galClassSize;   // byte size
  qint32   catNamesSize;   // byte size
} dsoHeader_t;

// text segment (char array)
// name1\tname2\tnameN\0 etc.

// galaxy class types (char array)
// S0\0E\0 etc.

// catalogue names (char array)
// NGC\0PGC\0

typedef struct
{
  qint32          nameOffs;
  radec_t         rd;         // in rads in FK5 (J2000.0)

  signed   short  mag;       // mag * 100
  unsigned short  pa;        // in degs (0..180)

  quint32         sx;        // in arc sec
  quint32         sy;        // in arc sec

  unsigned char   type;      // DSOT_xxx
  unsigned char   cataloque; // index to catalogue name

  unsigned short  shape;     // shape index

  unsigned short  galType;   // index to galaxy class
} dso_t;


#endif
