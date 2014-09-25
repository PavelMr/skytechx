#ifndef CGALERY_H
#define CGALERY_H

#include <QtGui>
#include <QtWidgets>

#include "cskpainter.h"
#include "cdb.h"

typedef struct
{
  QPixmap   pix;
  QString   path;
  QString   name;
  bool      found;
} pixmap_t;

Q_DECLARE_METATYPE(pixmap_t)

class CGalleryArea : public QAbstractScrollArea
{
  Q_OBJECT

  public:
    CGalleryArea(QWidget *parent = nullptr);

  protected slots:
    void slotLoad(const QPixmap &pix, const QString &name, const QString &path, bool found);
    void slotDoubleClicked(QModelIndex);
    void slotDeleteItem();

  private:
    QListWidget *m_list;
    QVBoxLayout *m_layout;
};


class CGalLoader : public QThread
{
  Q_OBJECT

public:
  void run(void);

  QString m_id;

signals:
  void sigLoad(const QPixmap &pix, const QString &name, const QString &path, bool found);
};

namespace Ui {
class CGalery;
}

class CGalery : public QDialog
{
  Q_OBJECT

public:
  explicit CGalery(QWidget *parent, const QString &id, const QString &name);
  ~CGalery();

protected:
  void changeEvent(QEvent *e);

  CGalleryArea     *m_area;
  CGalLoader        m_load;
  QString           m_id;
  QString           m_name;

private:
  Ui::CGalery *ui;

signals:
  void sigLoad(const QPixmap &pix, const QString &name, const QString &path, bool found);

private slots:

  void on_pushButton_2_clicked();
  void on_pushButton_clicked();
};

#endif // CGALERY_H
