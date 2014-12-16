#include "cobjinfo.h"
#include "ui_cobjinfo.h"
#include "cteleplug.h"
#include "cgalery.h"
#include "cobjtracking.h"
#include "cplanetrenderer.h"

#include <QPrintDialog>
#include <QPrinter>

/////////////////////////////////////
CObjInfo::CObjInfo(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CObjInfo)
/////////////////////////////////////
{
  ui->setupUi(this);
}

/////////////////////
CObjInfo::~CObjInfo()
/////////////////////
{
  QString note = ui->textEdit->toPlainText();
  QString str = "data/notes/" + m_infoItem.id + ".txt";

  str = str.simplified();
  SkFile f(str);

  if (note.length() == 0)
  {
    f.remove();
  }
  else
  {
    if (f.open(SkFile::WriteOnly | SkFile::Text))
    {
      QTextStream s(&f);

      s << note;
      f.close();
    }
  }

  //ui->textEdit->releaseKeyboard();

  delete ui;
}


//////////////////////////////////////////////////////////////////////////////
void CObjInfo::init(CMapView *map, const mapView_t *view, const mapObj_t *obj)
//////////////////////////////////////////////////////////////////////////////
{
  CObjFillInfo info;

  m_map = map;

  ui->clb_slew->setEnabled(g_pTelePlugin && (g_pTelePlugin->getAttributes() & TPI_CAN_SLEW));
  ui->clb_sync->setEnabled(g_pTelePlugin && (g_pTelePlugin->getAttributes() & TPI_CAN_SYNC));
  ui->clb_tracking->setEnabled(obj->type == MO_PLANET ||
                               obj->type == MO_ASTER ||
                               obj->type == MO_COMET);

  ui->cb_simbad->setDisabled(obj->type == MO_PLANET ||
                             obj->type == MO_ASTER ||
                             obj->type == MO_COMET ||
                             obj->type == MO_PLN_SAT);

  ui->pushButton_2->setDisabled(obj->type == MO_PLANET ||
                                obj->type == MO_ASTER ||
                                obj->type == MO_COMET ||
                                obj->type == MO_PLN_SAT);

  /*
  ui->clb_rise->setEnabled(false);
  ui->clb_set->setEnabled(false);
  ui->clb_transit->setEnabled(false);
  */

  info.fillInfo(view, obj, &m_infoItem);
  //ui->textEdit->grabKeyboard();

  /// read note
  QString str = "data/notes/" + m_infoItem.id + ".txt";
  SkFile f(str);
  if (f.open(SkFile::ReadOnly | SkFile::Text))
  {
    ui->textEdit->setPlainText(f.readAll());
    f.close();
  }
  QTextCursor crs(ui->textEdit->textCursor());
  crs.movePosition(QTextCursor::End);
  ui->textEdit->setTextCursor(crs);
  /////////////

  QFont fnt = ui->textBrowser1->font();

  fnt.setFamily("courier new");
  fnt.setPointSize(fnt.pointSize() + 1);
  ui->textBrowser1->setFont(fnt);
  ui->textBrowser_2->setFont(fnt);

  fillInfo(view, &m_infoItem);

  ui->textEdit->setFocus();

  if (m_infoItem.type == MO_PLANET && m_infoItem.par1 != PT_EARTH_SHADOW)
  {
    m_pixmapWidget = new CPixmapWidget();
    ui->gridLayout_5->addWidget(m_pixmapWidget);
    m_pixmapWidget->setPixmap(getPlanetPixmap(m_pixmapWidget->width(), m_pixmapWidget->height()));

    CXMLSimpleMapParser parser;

    parser.begin(readAllFile("data/planets/" + CAstro::getFileName(m_infoItem.par1) + ".xml"));

    addPhysicalInfo(&parser, "Equator_radius", tr("Equator radius"));
    addPhysicalInfo(&parser, "Semi_major_axis", tr("Semi-major axis"));
    addPhysicalInfo(&parser, "Eccentricity", tr("Eccentricity"));
    addPhysicalInfo(&parser, "Mass", tr("Mass"));
    addPhysicalInfo(&parser, "Rotation_period", tr("Rotation period"));
    addPhysicalInfo(&parser, "Escape_velocity", tr("Escape velocity"));
  }
  else
  {
    ui->tabWidget->blockSignals(true);
    ui->tabWidget->removeTab(1);
    ui->tabWidget->removeTab(1);
    ui->tabWidget->blockSignals(false);
  }
}


/////////////////////////////////////
void CObjInfo::changeEvent(QEvent *e)
/////////////////////////////////////
{
  QDialog::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
    ui->retranslateUi(this);
    break;
  default:
    break;
  }
}


////////////////////////////////////////////////
QString CObjInfo::getSpaces(int len, int maxLen)
////////////////////////////////////////////////
{
  QString str;
  int     c = maxLen - len;

  for (int i = 0; i < c; i++)
    str += "&nbsp;";

  return(str);
}

QPixmap CObjInfo::getPlanetPixmap(int w, int h)
{
  orbit_t o, s;
  SKPOINT pt;
  mapView_t view = m_map->m_mapView;

  pt.sx = w / 2.;
  pt.sy = h / 2.;

  cAstro.calcPlanet(m_infoItem.par1, &o);
  cAstro.calcPlanet(PT_SUN, &s);

  view.roll = 0;
  view.flipX = 0;
  view.flipY = 0;
  view.fov = D2R(o.sx / 3600.) * 1.2 * (w / (double)h) * (m_infoItem.par1 == PT_SATURN ? 1.7 : 1);

  QImage     i(w, h, QImage::Format_ARGB32);
  i.fill(Qt::black);
  CSkPainter p(&i);

  p.setRenderHint(QPainter::Antialiasing);
  p.setRenderHint(QPainter::SmoothPixmapTransform);
  ui->tab_2->setEnabled(true);
  trfCreateMatrixView(&cAstro, &view, w, h);
  cPlanetRenderer.renderPlanet(&pt, &o, &s, &view, &p, &i, 0, true);

  return QPixmap::fromImage(i);
}

void CObjInfo::resizeEvent(QResizeEvent *)
{
  if (m_infoItem.type == MO_PLANET && m_infoItem.par1 != PT_EARTH_SHADOW)
  {
    m_pixmapWidget->setPixmap(getPlanetPixmap(m_pixmapWidget->width(), qMin(m_pixmapWidget->height(), m_pixmapWidget->width())));
  }
}

void CObjInfo::addPhysicalInfo(CXMLSimpleMapParser *parser, const QString &mapIndex, const QString &label)
{
  if (parser->m_data.contains(mapIndex))
  {
    ui->textBrowser_2->setHtml(ui->textBrowser_2->toHtml() + "<b>" + label + "</b>" + getSpaces(label.length(), 28) + " : " + parser->m_data[mapIndex] + "\n");
  }
}

///////////////////////////////////////////////////////////
void CObjInfo::fillInfo(const mapView_t *, ofiItem_t *info)
///////////////////////////////////////////////////////////
{
  QString str;

  setWindowTitle(tr("Object information : ") + info->title);

  for (int i = 0; i < info->tTextItem.count(); i++)
  {
    ofiTextItem_t *item = &info->tTextItem[i];

    if (item->bBold)
      str += "<b>";

    if (item->bIsTitle)
      str += "<u>";

    str += item->label;

    if (item->bIsTitle)
      str += "</u>";

    if (!item->bIsTitle)
    {
      str += getSpaces(item->label.length(), 28);
      if (item->value.length() > 0)
        str += " : ";
      str += item->value;
    }

    if (item->bBold)
      str += ("</b>");

    str += ("<br/>");
  }

  ui->textBrowser1->setHtml(str);

  QTextCursor crs(ui->textBrowser1->textCursor());
  crs.setPosition(0);
  ui->textBrowser1->setTextCursor(crs);
}


//////////////////////////////////
// OK
void CObjInfo::on_clb_ok_clicked()
//////////////////////////////////
{
  done(DL_OK);
}


//////////////////////////////////////
// center
void CObjInfo::on_clb_center_clicked()
//////////////////////////////////////
{
  double ra = m_infoItem.radec.Ra;
  double dec = m_infoItem.radec.Dec;

  precess(&ra, &dec, JD2000, m_map->m_mapView.jd);

  m_map->centerMap(ra, dec, CM_UNDEF);
  done(DL_OK);
}

///////////////////////////////////////////
// center & zoom
void CObjInfo::on_clb_center_zoom_clicked()
///////////////////////////////////////////
{
  double ra = m_infoItem.radec.Ra;
  double dec = m_infoItem.radec.Dec;

  precess(&ra, &dec, JD2000, m_map->m_mapView.jd);

  m_map->centerMap(ra, dec, m_infoItem.zoomFov);
  done(DL_OK);
}


//////////////////////////////////////
// delete note
void CObjInfo::on_pushButton_clicked()
//////////////////////////////////////
{
  ui->textEdit->clear();
  ui->textEdit->setFocus();
}


////////////////////////////////////
// slew telescope
void CObjInfo::on_clb_slew_clicked()
////////////////////////////////////
{
  radec_t rd;

  precess(&m_infoItem.radec, &rd, JD2000, m_map->m_mapView.jd);

  double r = R2D(rd.Ra) / 15.0;
  double d = R2D(rd.Dec);

  QApplication::beep();

  g_pTelePlugin->slewTo(r, d);
}


///////////////////////////////////////
void CObjInfo::on_clb_gallery_clicked()
///////////////////////////////////////
{
  CGalery dlg(this, m_infoItem.id, m_infoItem.title);

  dlg.exec();
}


////////////////////////////////////////
void CObjInfo::on_clb_tracking_clicked()
////////////////////////////////////////
{
  CObjTracking dlg(this, &m_infoItem, &m_map->m_mapView);

  dlg.exec();
}


////////////////////////////////////
void CObjInfo::on_clb_sync_clicked()
////////////////////////////////////
{
  radec_t rd;

  precess(&m_infoItem.radec, &rd, JD2000, m_map->m_mapView.jd);

  double r = R2D(rd.Ra) / 15.0;
  double d = R2D(rd.Dec);

  QApplication::beep();

  g_pTelePlugin->syncTo(r, d);
}


////////////////////////////////////
void CObjInfo::on_cb_print_clicked()
////////////////////////////////////
{
  QPrinter prn;
  QPrintDialog dlg(&prn, this);

  if (dlg.exec() == DL_CANCEL)
    return;

  ui->textBrowser1->print(&prn);
}

/////////////////////////////////////
void CObjInfo::on_cb_simbad_clicked()
/////////////////////////////////////
{
  QString url;

  url = QString("http://simbad.u-strasbg.fr/simbad/sim-id?output.format=html&Ident=%1&obj.notesel=off").arg(m_infoItem.simbad);

  QDesktopServices::openUrl(url);
}

void CObjInfo::on_pushButton_2_clicked()
{
  QString url;

  url = QString("http://ned.ipac.caltech.edu/cgi-bin/nph-objsearch?extend=no&out_csys=Equatorial&out_equinox=J2000.0&obj_sort=RA+or+Longitude&list_limit=10&img_stamp=YES&objname=%1").arg(m_infoItem.simbad);

  QDesktopServices::openUrl(url);
}

////////////////////////////////////
void CObjInfo::on_cb_copy1_clicked()
////////////////////////////////////
{
  QClipboard *clipboard = QApplication::clipboard();
  double ra, dec;

  ra = m_infoItem.radec.Ra;
  dec = m_infoItem.radec.Dec;

  precess(&ra, &dec, JD2000, m_map->m_mapView.jd);

  QString     str = getStrRA(ra) + " "  + getStrDeg(dec);

  clipboard->setText(str);
}

////////////////////////////////////
void CObjInfo::on_cb_copy2_clicked()
////////////////////////////////////
{
  QClipboard *clipboard = QApplication::clipboard();

  clipboard->setText(m_infoItem.title);
}

void CObjInfo::on_tabWidget_currentChanged(int)
{
  m_pixmapWidget->setPixmap(getPlanetPixmap(m_pixmapWidget->width(), m_pixmapWidget->height()));
}


