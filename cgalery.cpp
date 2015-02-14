#include "cgalery.h"
#include "ui_cgalery.h"
#include "cimageview.h"

#define THM_SIZE     236

static QWidget *pParent;
static QString objId;

static QString makeString(QString cmd)
{
  return cmd.replace("'", "''");
}

CGalery::CGalery(QWidget *parent, const QString &id, const QString &name) :
  QDialog(parent),
  ui(new Ui::CGalery)
{
  ui->setupUi(this);

  objId = id;
  m_id = id;
  m_name = name;
  pParent = this;

  setWindowTitle(tr("Galery of ") + m_name);

  m_area = new CGalleryArea();
  QHBoxLayout *layout = new QHBoxLayout(ui->frame);

  layout->addWidget(m_area);

  connect(&m_load, SIGNAL(sigLoad(QPixmap,QString,QString,bool)), m_area, SLOT(slotLoad(QPixmap, QString, QString, bool)));
  connect(this, SIGNAL(sigLoad(QPixmap, QString, QString, bool)), m_area, SLOT(slotLoad(QPixmap, QString, QString, bool)));

  m_load.m_id = id;
  m_load.start();

}

///////////////////
CGalery::~CGalery()
///////////////////
{
  delete ui;
}

////////////////////////////////////
void CGalery::changeEvent(QEvent *e)
////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
CGalleryArea::CGalleryArea(QWidget *parent) : QAbstractScrollArea(parent)
/////////////////////////////////////////////////////////////////////////
{
  m_list = new QListWidget();
  m_list->setViewMode(QListWidget::IconMode);
  m_list->setMovement(QListView::Static);
  m_list->setResizeMode(QListView::Adjust);
  m_list->setIconSize(QSize(256, 256));

  connect(m_list, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotDoubleClicked(QModelIndex)));

  QShortcut *sh1 = new QShortcut(QKeySequence(Qt::Key_Delete), m_list, 0, 0,  Qt::WidgetShortcut);
  connect(sh1, SIGNAL(activated()), this, SLOT(slotDeleteItem()));

  m_layout = new QVBoxLayout(this);
  m_layout->setMargin(4);
  m_layout->addWidget(m_list);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void CGalleryArea::slotLoad(const QPixmap &pix, const QString &name, const QString &path, bool found)
/////////////////////////////////////////////////////////////////////////////////////////////////////
{
  pixmap_t p;

  p.pix = pix;
  p.name = name;
  p.path = path;
  p.found = found;

  QListWidgetItem* item = new QListWidgetItem(QIcon(p.pix), name);

  QVariant var;
  var.setValue(p);

  item->setData(Qt::UserRole, var);

  m_list->addItem(item);
}

///////////////////////////////////////////////////////
void CGalleryArea::slotDoubleClicked(QModelIndex index)
///////////////////////////////////////////////////////
{
  pixmap_t pixmap = index.data(Qt::UserRole).value<pixmap_t>();

  if (!pixmap.found)
  {
    return;
  }

  QDialog  dlg(pParent);
  QPixmap  pix(pixmap.path);

  CImageView img(&dlg);

  dlg.showFullScreen();
  img.resize(dlg.size());
  img.setSource(&pix);
  dlg.exec();

}

///////////////////////////////////
void CGalleryArea::slotDeleteItem()
///////////////////////////////////
{
  QList <QListWidgetItem *> selected = m_list->selectedItems();

  if (selected.isEmpty())
  {
    return;
  }
  pixmap_t pixmap = selected[0]->data(Qt::UserRole).value<pixmap_t>();

  QSqlQuery q(*g_pDb);
  QString cmd = QString("delete from gallery where name='" + makeString(objId) + "'" + " and path='" + pixmap.path + "'");

  q.exec(cmd);
  delete selected[0];
}

//////////////////////
void CGalLoader::run()
//////////////////////
{
  QSqlQuery q(*g_pDb);
  QString cmd = "select * from gallery where name='" + makeString(m_id) + "'";

  q.exec(cmd);

  qDebug() << cmd;

  int imgNo = q.record().indexOf("path");

  while (q.next())
  {
    QString path = q.value(imgNo).toString();

    QPixmap pixOri(path);
    QPixmap pix;
    bool found;

    if (!pixOri.isNull())
    {
      pix = pixOri.scaled(THM_SIZE, THM_SIZE, Qt::KeepAspectRatio);
      found = true;
    }
    else
    {
      QPixmap emptyPix(240, 64);
      emptyPix.fill(Qt::black);
      QPainter p(&emptyPix);

      p.setPen(Qt::white);
      p.drawText(QRect(0, 0, emptyPix.width(), emptyPix.height()), Qt::AlignCenter, tr("Image was not found!!!"));

      pix = emptyPix;
      found = false;
    }

    QFileInfo fi(path);

    emit sigLoad(pix, fi.fileName(), path, found);
  }
}

///////////////////////////////////////
// add
void CGalery::on_pushButton_2_clicked()
///////////////////////////////////////
{
  QFileDialog dlg(this, tr("Open a File"), "data/", "All Image Files (*.jpg;*.bmp;*.png);;JPG file (*.jpg);;BMP file (*.bmp);;PNG file (*.png)");

  dlg.setAcceptMode(QFileDialog::AcceptOpen);
  dlg.setFileMode(QFileDialog::ExistingFiles);

  if (dlg.exec())
  {
    QStringList names = dlg.selectedFiles();

    QProgressDialog dlg(tr("Please wait..."), "", 0, names.count() - 1, this);

    dlg.setWindowFlags(((dlg.windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint));
    dlg.setWindowModality(Qt::WindowModal);
    dlg.setMinimumDuration(0);
    dlg.setCancelButton(NULL);
    dlg.show();

    for (int i = 0; i < names.count(); i++)
    {
      if (!g_pDb->addToGallery(m_name, m_id, names[i]))
      {
        msgBoxError(this, tr("Cannot insert to DB!!!"));
        break;
      }

      QPixmap pixOri(names[i]);
      QPixmap pix;
      bool found;

      if (!pixOri.isNull())
      {
        qDebug() << pix.width();
        pix = pixOri.scaled(THM_SIZE, THM_SIZE, Qt::KeepAspectRatio);
        found = true;
      }
      else
      {
        QPixmap emptyPix(256, 256);
        pix = emptyPix;
        found = false;
      }

      QFileInfo fi(names[i]);

      emit sigLoad(pix, fi.fileName(), names[i], found);

      dlg.setValue(i);
      QApplication::processEvents();
    }
  }
}


void CGalery::on_pushButton_clicked()
{
  done(DL_OK);
}
