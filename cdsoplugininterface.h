#ifndef CDSOPLUGININTERFACE_H
#define CDSOPLUGININTERFACE_H

#include "QString"
#include "QList"

class CDSOPluginInterface
{
public:

  typedef struct
  {
    QString  label;
    QString  value;
  } dsoPlgItem_t;

  typedef struct
  {
    QString title;
    QList <dsoPlgItem_t> items;
  } dsoPlgOut_t;

  virtual void init(QString path) = 0;
  virtual dsoPlgOut_t getDesc(const QString name1, const QString name2) = 0;
  virtual QString getTitle() = 0;
};

QT_BEGIN_NAMESPACE

Q_DECLARE_INTERFACE(CDSOPluginInterface, "Qt.Skytech.CDSOPluginInterface")

QT_END_NAMESPACE

#endif // CDSOPLUGININTERFACE_H
