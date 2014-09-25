#include "clfmodel.h"

CLFModel::CLFModel(int rows, int columns, QObject *parent) : QStandardItemModel(rows, columns, parent)
{  
}

void CLFModel::slotDataChange(const QModelIndex &, const QModelIndex &)
{
}
