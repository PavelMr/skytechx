#ifndef CFITS_H
#define CFITS_H

#include "QtCore"
#include "QtGui"

#include "skcore.h"

#define ARCSECONDS_PER_RADIAN (3600. * 180. / MPI)

class CFits
{
public:
    CFits();
   ~CFits();
    bool    load(QString name, bool &memOk, bool bAll = true, int resizeTo = 0);
    QString getValue(QString name, QString defVal = "");
    QImage *getImage(void);
    QImage *getOriginalImage();
    double  m_ra;
    double  m_dec;
    radec_t m_cor[4];
    radec_t cen_rd;
    QString m_name;
    bool    m_inverted;
    int     m_brightness;
    int     m_contrast;
    int     m_gamma;
    bool    m_bAuto;
    int     m_datamin;
    int     m_datamax;


protected:
   QImage *m_pix;
   QImage *m_ori;

   QMap <QString, QString> tFitsMap;
   void amdpos(double x, double y, double *ra, double *dec);

   double x_pixel_size;
   double y_pixel_size;
   double ppo_coef[6];
   double amd_x_coeff[20];
   double amd_y_coeff[20];
};

#endif // CFITS_H
