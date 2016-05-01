#ifndef CLVQINFO_H
#define CLVQINFO_H

#include <QtGui>
#include <QtCore>

#include "cobjfillinfo.h"

class CLvQInfo : public QTreeView
{
  Q_OBJECT
public:
  explicit CLvQInfo(QWidget *parent = 0);
          ~CLvQInfo();
  void     init(QToolBox *parent);

  void fillInfo(ofiItem_t *data, bool update = false);
  void removeInfo(void);

  ofiItem_t m_info;
  bool      m_bFilled;
  int       lastCount;
signals:

protected:
  QToolBox *m_parent;

public slots:

};

#endif // CLVQINFO_H
