#ifndef CLFMODEL_H
#define CLFMODEL_H

#include <QtGui>
#include <QtCore>

class CLFModel : public QStandardItemModel
{
  Q_OBJECT

public:
  CLFModel( int rows, int columns, QObject * parent = 0 );

public slots:
  void slotDataChange( const QModelIndex & topLeft, const QModelIndex & bottomRight);

};

#endif // CLFMODEL_H
