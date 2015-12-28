#include "cimageview.h"

/////////////////////////////////////////
CImageView::CImageView(QWidget *parent) :
/////////////////////////////////////////
  QWidget(parent)
{
  m_pix = NULL;
  m_firstTime = true;
  m_type = IV_NO_IMAGE;

  showFullScreen();
  showMaximized();

  setCursor(Qt::OpenHandCursor);

  m_waiting = new QMovie(":/res/load.gif");

  m_label = new QLabel(this);
  m_label->setAlignment(Qt::AlignCenter);

  m_btReset = new QPushButton("X", this);
  m_btReset->resize(m_btReset->height(), m_btReset->height());
  m_btReset->setCursor(QCursor(Qt::ArrowCursor));
  m_btReset->setToolTip(tr("Rescale to screen size"));

  connect(m_btReset, SIGNAL(clicked()), this, SLOT(slotReset()));

  m_bt100 = new QPushButton(tr("100%"), this);
  m_bt100->resize(60, m_bt100->height());
  m_bt100->setCursor(QCursor(Qt::ArrowCursor));
  m_bt100->setToolTip(tr("Resize to 100%"));

  connect(m_bt100, SIGNAL(clicked()), this, SLOT(slot100()));

  setAttribute(Qt::WA_OpaquePaintEvent, true);
  setAttribute(Qt::WA_NoSystemBackground, true);
}

/////////////////////////
CImageView::~CImageView()
/////////////////////////
{
  delete m_waiting;
  delete m_pix;
}

//////////////////////////////////////////
void CImageView::paintEvent(QPaintEvent *)
//////////////////////////////////////////
{
  QPainter p(this);

  p.fillRect(rect(), Qt::black);
  p.setRenderHint(QPainter::SmoothPixmapTransform);

  if (m_type == IV_IMAGE)
  {
    p.save();

    p.translate(width() / 2, height() / 2);
    p.scale(m_scale, m_scale);
    p.drawPixmap(-m_cx, -m_cy, *m_pix);

    p.restore();
  }
  else
  if (m_type == IV_NO_IMAGE)
  {
    p.setPen(Qt::white);
    p.drawText(rect(), Qt::AlignCenter, tr("No image to view!!!"));
  }
  else
  if (m_type == IV_WAITING)
  {
  }
}

void CImageView::resizeEvent(QResizeEvent *)
{
  m_btReset->move(5, 5);
  m_bt100->move(40, 5);

  if (m_firstTime)
  {
    slotReset();
  }

  m_firstTime = false;
}

////////////////////////////////////////////////
void CImageView::mousePressEvent(QMouseEvent *e)
////////////////////////////////////////////////
{
  m_lastX = e->pos().x();
  m_lastY = e->pos().y();

  setCursor(Qt::ClosedHandCursor);
}

/////////////////////////////////////////////////
void CImageView::mouseReleaseEvent(QMouseEvent *)
/////////////////////////////////////////////////
{
  setCursor(Qt::OpenHandCursor);
}

///////////////////////////////////////////////
void CImageView::mouseMoveEvent(QMouseEvent *e)
///////////////////////////////////////////////
{
  m_cx -= (e->pos().x() - m_lastX) / m_scale;
  m_cy -= (e->pos().y() - m_lastY) / m_scale;

  m_lastX = e->pos().x();
  m_lastY = e->pos().y();

  update();
}

///////////////////////////////////////////
void CImageView::wheelEvent(QWheelEvent *e)
///////////////////////////////////////////
{
  if (e->delta() > 0)
  {
    m_scale *= 1.1;
    if (m_scale > 8)
      m_scale = 8;
  }
  else
  {
    m_scale *= 0.9;
    if (m_scale < 0.2)
      m_scale = 0.2;
  }

  update();
}

////////////////////////////
void CImageView::slotReset()
////////////////////////////
{
  if (m_type == IV_NO_IMAGE)
  {
    return;
  }

  m_cx = m_pix->width() / 2;
  m_cy = m_pix->height() / 2;
  m_scale = qMin(width() / (double)m_pix->width(), height() / (double)m_pix->height());

  update();
}


//////////////////////////
void CImageView::slot100()
//////////////////////////
{
  if (m_type == IV_NO_IMAGE)
  {
    return;
  }

  m_cx = m_pix->width() / 2;
  m_cy = m_pix->height() / 2;
  m_scale = 1;

  update();
}

///////////////////////////////////
void CImageView::setSourceNoImage()
///////////////////////////////////
{
  m_btReset->setEnabled(false);
  m_bt100->setEnabled(false);

  m_waiting->stop();
  m_label->hide();

  SAFE_DELETE(m_pix);
  m_type = IV_NO_IMAGE;
  update();
}

///////////////////////////////////
void CImageView::setSourceWaiting()
///////////////////////////////////
{
  m_btReset->setEnabled(false);
  m_bt100->setEnabled(false);

  m_label->show();
  m_label->move(0, 0);
  m_label->resize(size());
  m_label->setMovie(m_waiting);
  m_waiting->start();

  SAFE_DELETE(m_pix);
  m_type = IV_WAITING;

  update();
}

////////////////////////////////
QPixmap *CImageView::getSource()
////////////////////////////////
{
  return(m_pix);
}

void CImageView::setOriginalSize()
{
  slot100();
}

////////////////////////////////////////
void CImageView::setSource(QPixmap *pix)
////////////////////////////////////////
{
  SAFE_DELETE(m_pix);

  m_pix = new QPixmap(*pix);
  m_type = IV_IMAGE;

  m_waiting->stop();
  m_label->hide();

  m_cx = m_pix->width() / 2;
  m_cy = m_pix->height() / 2;
  m_scale = qMin(width() / (double)m_pix->width(), height() / (double)m_pix->height());

  m_btReset->setEnabled(true);
  m_bt100->setEnabled(true);
}
