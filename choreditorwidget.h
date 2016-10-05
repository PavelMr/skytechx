#ifndef CHOREDITORWIDGET_H
#define CHOREDITORWIDGET_H

#include <QWidget>

namespace Ui {
class CHorEditorWidget;
}

class CHorEditorWidget : public QWidget
{
  Q_OBJECT

public:
  explicit CHorEditorWidget(QWidget *parent = 0);
  ~CHorEditorWidget();

private:
  Ui::CHorEditorWidget *ui;
};

#endif // CHOREDITORWIDGET_H
