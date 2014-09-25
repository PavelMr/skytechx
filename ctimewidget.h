#ifndef CTIMEWIDGET_H
#define CTIMEWIDGET_H

#include <QWidget>

#include "cmapview.h"

namespace Ui {
class CTimeWidget;
}

class CTimeWidget : public QWidget
{
  Q_OBJECT

public:
  explicit CTimeWidget(QWidget *parent = 0);
  ~CTimeWidget();
  void timeUpdate(mapView_t *view);

protected:
  void changeEvent(QEvent *e);

private:
  Ui::CTimeWidget *ui;
};

#endif // CTIMEWIDGET_H
