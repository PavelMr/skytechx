#ifndef CPPMXL_H
#define CPPMXL_H

#include <QtCore>
#include "skcore.h"

#pragma pack(2)

typedef struct
{
  qint64 id;
    uint ra;
     int dec;
   short mag;
}  ppmxl_t;

#pragma pack()

typedef struct
{
  int      count;
  int      regNo;
  ulong    timer;
  ppmxl_t *data;
} ppmxlCache_t;

class CPPMXL
{
public:
    CPPMXL();
   ~CPPMXL();
    ppmxlCache_t *getRegion(int gscReg);
    void setDir(QString dir);

protected:
    QString m_ppmxlDir;
};

extern CPPMXL           cPPMXL;

#endif // CPPMXL_H
