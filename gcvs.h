#ifndef GCVS_H
#define GCVS_H

#include <QObject>
#include <QtGlobal>
#include <QList>
#include <QMap>

#define MAKE_TYC(t1, t2, t3)      (((qint64)t3 << 32) | ((qint64)t2 << 16) | ((qint64)t1))

typedef struct
{
  qint16  tyc1;
  qint16  tyc2;
  qint8   tyc3;

  QString name;
  QString type;
  float   magMax;
  qint8   magMaxSymbol;
  float   magMin;
  qint8   magMinSymbol;
  double  epoch;
  double  period;

  // not part of data file
  QString typeDesc;
} gcvs_t;

class GCVS : public QObject
{
  Q_OBJECT
public:
  GCVS();
  void load();
  gcvs_t *getStar(qint16 tyc1, qint16 tyc2, qint8 tyc3);
  bool findStar(const QString &name, gcvs_t *star);
  QStringList nameList();
  double solveNextMaximum(double epoch, double period, double jd);
  double solveNextMinimum(double epoch, double period, double jd);

  QList<gcvs_t> getList() const;

  QString getTypes(int index);
private:
  QList <gcvs_t> m_list;
  QMap  <qint64, int> m_map;

  QString getTypeDesc(const QString &type);
};

extern GCVS g_GCVS;

#endif // GCVS_H
