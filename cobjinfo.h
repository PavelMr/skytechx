#ifndef COBJINFO_H
#define COBJINFO_H

#include <QtCore>
#include <QtGui>
#include <QtNetwork>
#include <QResizeEvent>

#include "cobjfillinfo.h"
#include "cmapview.h"
#include "cpixmapwidget.h"
#include "cxmlsimplemapparser.h"

namespace Ui {
class CObjInfo;
}

class CObjInfo : public QDialog
{
  Q_OBJECT

public:
  explicit CObjInfo(QWidget *parent = 0);
  ~CObjInfo();
  void init(CMapView *map, const mapView_t *view, const mapObj_t *obj);

  ofiItem_t              m_infoItem;

protected:
  void changeEvent(QEvent *e);
  void fillInfo(const mapView_t *view, ofiItem_t *info);
  QString getSpaces(int len, int maxLen);
  QPixmap getPlanetPixmap(int w, int h);
  void resizeEvent(QResizeEvent *);
  QString addPhysicalInfo(CXMLSimpleMapParser *parser, const QString& mapIndex, const QString& label);

  CMapView              *m_map;
  CPixmapWidget         *m_pixmapWidget;

private:
  Ui::CObjInfo *ui;

public slots:

private slots:
  void on_clb_ok_clicked();
  void on_clb_center_clicked();
  void on_clb_center_zoom_clicked();
  void on_pushButton_clicked();
  void on_clb_slew_clicked();
  void on_clb_gallery_clicked();  
  void on_clb_sync_clicked();
  void on_cb_print_clicked();
  void on_cb_simbad_clicked();
  void on_cb_copy1_clicked();
  void on_cb_copy2_clicked();
  void on_tabWidget_currentChanged(int index);
  void on_pushButton_2_clicked();
  void on_pushButton_3_clicked();
};

#endif // COBJINFO_H
