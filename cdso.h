#ifndef CDSO_H
#define CDSO_H

#include "QtCore"
#include "skcore.h"
#include "dso_def.h"
#include "transform.h"
#include "cmapview.h"
#include "cskpainter.h"
#include "cshape.h"

#define NUM_DSO_SEG_Y    24
#define NUM_DSO_SEG_X    48

#define DSO_SEG_SIZE     7.5

typedef struct
{
  SKPOINT p[4];
} dsoRect_t;

typedef struct
{
  char     catName[32];
  QString  commonName;
} dsoCommonName_t;

typedef struct
{
  QString label;
  int     sx, sy;
} dsoLabel_t;

class CDso : public QObject
{
  Q_OBJECT

  public:
    CDso();
   ~CDso();
    void load();
    void createSectors();
    void setPainter(CSkPainter *p, QImage *pImg);
    void renderObj(SKPOINT *pt, dso_t *pDso, mapView_t *mapView);
    int  findDSO(char *pszName, dso_t **pDso);
    int  findDSOFirstName(char *pszName);
    QString getTypeName(int type, bool &ok);
    QString getCatalogue(dso_t *pDso);
    QString getCatalogue(int index);
    QString getClass(dso_t *pDso);

    QString getCommonName(dso_t *pDso);
    QStringList getNameList(dso_t *pDso);
    QString getName(dso_t *pDso);
    QString getName(dso_t *pDso, int idx);
    QStringList getNameInt(dso_t *pDso);

    bool addAddon(const QString name);

    void drawShape(QPainter *p, QImage *img, dso_t *dso, mapView_t *view);
    void renderNebulaSymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter);
    void renderOpenClsSymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter);
    void renderGlobClsSymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter);
    void renderPlnNebulaSymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter);
    void renderGalaxySymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter, mapView_t *mapView);
    void renderGalaxyClsSymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter);
    void renderDsoStarSymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter);
    void renderOtherSymbol(SKPOINT *pt, dso_t *pDso, CSkPainter *pPainter);

    //TODO: dat nazvy do QStringList *dsoNames;
    dsoHeader_t        dsoHead;
    dso_t           *dso;
    char            *dsoNames;
    char            *dsoClass;
    char            *dsoCats;

    QHash <int, QStringList> namesMap;

    QList <dsoLabel_t> tLabels;
    QList <CShape *>   tShapeList;
    dsoRect_t          sector[NUM_DSO_SEG_Y][NUM_DSO_SEG_X];
    QList <int>        tDsoSectors[NUM_DSO_SEG_Y][NUM_DSO_SEG_X];
    QList              <dsoCommonName_t> tDsoCommonNames;

protected:
    CSkPainter      *pPainter;
    QImage          *pImg;
    int              m_fntHeight;

    QPen             m_pen;

    void loadNames();
    void loadShapes();
};

extern CDso  cDSO;

#endif // CDSO_H
