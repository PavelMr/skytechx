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
  QString str = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "../data/notes/" + m_infoItem.id + ".txt";

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

  delete ui;
}


//////////////////////////////////////////////////////////////////////////////
void CObjInfo::init(CMapView *map, const mapView_t *view, const mapObj_t *obj)
//////////////////////////////////////////////////////////////////////////////
{
  CObjFillInfo info;

  m_map = map;

  ui->pushButton_3->setEnabled(false);
  ui->clb_slew->setEnabled(g_pTelePlugin && (g_pTelePlugin->getAttributes() & TPI_CAN_SLEW));
  ui->clb_sync->setEnabled(g_pTelePlugin && (g_pTelePlugin->getAttributes() & TPI_CAN_SYNC));
  ui->clb_tracking->setEnabled(obj->type == MO_PLANET ||
                               obj->type == MO_ASTER ||
                               obj->type == MO_COMET ||
                               obj->type == MO_SATELLITE);

  ui->cb_simbad->setDisabled(obj->type == MO_PLANET ||
                             obj->type == MO_ASTER ||
                             obj->type == MO_COMET ||
                             obj->type == MO_PLN_SAT ||
                             obj->type == MO_SATELLITE);

  ui->pushButton_2->setDisabled(obj->type == MO_PLANET ||
                                obj->type == MO_ASTER ||
                                obj->type == MO_COMET ||
                                obj->type == MO_PLN_SAT ||
                                obj->type == MO_SATELLITE);

  ui->pushButton_3->setEnabled(obj->type == MO_ASTER);


  info.fillInfo(view, obj, &m_infoItem);

  /// read note
  QString str = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "../data/notes/" + m_infoItem.id + ".txt";
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

  fillInfo(view, &m_infoItem);

  ui->textEdit->setFocus();

  if (m_infoItem.type == MO_PLANET && m_infoItem.par1 != PT_EARTH_SHADOW)
  {
    m_pixmapWidget = new CPixmapWidget();
    ui->gridLayout_5->addWidget(m_pixmapWidget);
    m_pixmapWidget->setPixmap(getPlanetPixmap(m_pixmapWidget->width(), m_pixmapWidget->height()));

    CXMLSimpleMapParser parser;

    parser.begin(readAllFile("../data/planets/" + CAstro::getFileName(m_infoItem.par1) + ".xml"));

    QString str = "<table>\n";

    str += addPhysicalInfo(&parser, "Equator_radius", tr("Equator radius"));
    str += addPhysicalInfo(&parser, "Semi_major_axis", tr("Semi-major axis"));
    str += addPhysicalInfo(&parser, "Eccentricity", tr("Eccentricity"));
    str += addPhysicalInfo(&parser, "Mass", tr("Mass"));
    str += addPhysicalInfo(&parser, "Rotation_period", tr("Rotation period"));
    str += addPhysicalInfo(&parser, "Escape_velocity", tr("Escape velocity"));

    str +=  "</table>\n";

    ui->textBrowser_2->setHtml(str);
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

  cAstro.setParam(&view);
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

QString CObjInfo::addPhysicalInfo(CXMLSimpleMapParser *parser, const QString &mapIndex, const QString &label)
{
  QString str;
  if (parser->m_data.contains(mapIndex))
  {
    str += "<tr><td><b>" + label + "</b></td><td> &nbsp;&nbsp; : &nbsp;&nbsp; </td> <td>" + parser->m_data[mapIndex] + "</td></tr>\n";
  }

  return str;
}

///////////////////////////////////////////////////////////
void CObjInfo::fillInfo(const mapView_t *, ofiItem_t *info)
///////////////////////////////////////////////////////////
{
  QString str;

  setWindowTitle(tr("Object information : ") + info->title);

  str += "<table>\n";

  for (int i = 0; i < info->tTextItem.count(); i++)
  {
    ofiTextItem_t *item = &info->tTextItem[i];

    str += "<tr>\n";

    str += "<td>\n";

    if (item->bBold)
      str += "<b>";

    if (item->bIsTitle)
      str += "<u>";

    str += item->label;

    if (item->bIsTitle)
      str += "</u>";

    str += "</td>\n";

    if (!item->bIsTitle)
    {
      str += "<td>\n";
      str += "&nbsp;&nbsp; : &nbsp;&nbsp;";
      str += "</td>\n";

      str += "<td>\n";
      str += item->value;
      str += "</td>\n";
    }

    if (item->bBold)
      str += ("</b>");

    str += "</tr>\n";
  }

  str += "</table>\n";

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

  // TODO: udelat to ne takhle prasacky
  setWindowOpacity(0);
  dlg.exec();
  setWindowOpacity(1);
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



void CObjInfo::on_pushButton_3_clicked()
{
  QString str = m_infoItem.simbad;;

  str = str.remove(QRegExp("\\((.*)\\)")).simplified();

  QUrl url(QString("http://ssd.jpl.nasa.gov/sbdb.cgi?sstr=%1;orb=0;cov=0;log=0;cad=0#top").arg(str));
  QDesktopServices::openUrl(url);
}




















