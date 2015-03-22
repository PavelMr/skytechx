#include "chorizonwidget.h"
#include "background.h"

CHorizonWidget::CHorizonWidget(QWidget *parent) :
  QFrame(parent),
  m_editor(NULL),
  m_drag(false),
  m_labelDrag(-1)
{
  setMouseTracking(true);

  m_azm = 0;
  m_alt = 0;

  m_map[90] = 10;
  m_map[270] = 20;

  background.isValid = true;
  background.isTexture = false;

  updateData();

  background.makeHorizon(&m_list, m_altHorizon);

  QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Preferred);
  sp.setHeightForWidth(true);
  setSizePolicy(sp);
}

int CHorizonWidget::heightForWidth(int w) const
{
  //qDebug() << w;
  return w / 2;
}

bool CHorizonWidget::load(const QString &name)
{
  m_label.clear();
  m_map.clear();
  background.resetBackground();

  memset(m_altHorizon, 0, sizeof(m_altHorizon));

  if (background.loadBackground(name))
  {
    if (background.isValid)
    {
      m_map = background.altMap;
    }

    if (background.isTexture)
    {
      m_textureName = background.textureName;
    }

    for (int i = 0; i < background.bkNames.count(); i++)
    {
      label_t label;

      label.azm = background.bkNames[i].azm;
      label.label = background.bkNames[i].name;

      m_label.append(label);
    }

    updateData();
    update();
  }
  else
  {
    return false;
  }
  return true;
}

bool CHorizonWidget::save(const QString &name)
{

}

void CHorizonWidget::removeTexture()
{
  if (background.isTexture)
  {
    delete background.bkTexture;
    background.bkTexture = NULL;
    background.isTexture = false;

    if (!background.isValid)
    {
      m_map[90] = 0;
      m_map[270] = 0;

      background.isValid = true;
      background.isTexture = false;

      updateData();

      background.makeHorizon(&m_list, m_altHorizon);
    }

    update();
  }
}

void CHorizonWidget::setTexture(const QString &name)
{
  if (background.isTexture)
  {
    delete background.bkTexture;
  }

  m_textureName = name;
  background.bkTexture = new QImage(name);

  if (!background.bkTexture->isNull())
  {
    background.isTexture = true;
    updateData();
  }
  else
  {
    delete background.bkTexture;
    background.isTexture = false;
  }

  update();
}

void CHorizonWidget::paintEvent(QPaintEvent *)
{
  QPainter p(this);

  p.setRenderHint(QPainter::Antialiasing, true);
  p.fillRect(rect(), Qt::black);

  if (background.isTexture)
  {
    p.drawImage(rect(), *background.bkTexture);
  }

  p.setPen(QPen(Qt::white, 1, Qt::DotLine));
  p.drawLine(0, height() / 2 , width(), height() / 2);
  double x, y;

  getPos(90, 0, x, y);
  p.drawLine(x, 0 , x, height());
  getPos(180, 0, x, y);
  p.drawLine(x, 0 , x, height());
  getPos(270, 0, x, y);
  p.drawLine(x, 0 , x, height());

  p.drawText(10, 10, 1000, 1000, Qt::AlignLeft || Qt::AlignBottom, QString::number(m_azm) + QString("° %1°").arg(m_alt, 0, 'f', 2));

  if (background.isValid)
  {
    for (int i = 0; i < 359; i++)
    {
      double x1, y1;
      double x2, y2;

      getPos(i, R2D(m_altHorizon[i]), x1, y1);
      getPos(i + 1, R2D(m_altHorizon[i + 1]), x2, y2);

      p.setPen(QPen(Qt::red, 2));
      if (m_map.contains(i))
      {
        p.drawEllipse(QPoint(x1, y1), 3, 3);
      }
      if (i == 358 && m_map.contains(359))
      {
        p.drawEllipse(QPoint(x2, y2), 3, 3);
      }

      p.setPen(QPen(Qt::white, 1));
      p.drawLine(QLineF(x1, y1, x2, y2));
    }
  }

  for (int i = 0; i < m_label.count(); i++)
  {
    int azm = m_label[i].azm;

    double x1, y1;
    double x2, y2;
    QRect rc;
    QRect rc2;

    getPos(azm, R2D(m_altHorizon[azm]), x1, y1);
    getPos(azm, R2D(m_altHorizon[azm]) + 10, x2, y2);

    p.setPen(QPen(Qt::yellow, 2));
    p.drawLine(QLineF(x1, y1, x2, y2));

    QFontMetrics fm(p.font());

    rc = fm.boundingRect(m_label[i].label);
    rc.moveCenter(QPoint(x1, y2));
    rc2 = rc;

    rc.adjust(-2, -2, 2, 2);
    rc2.adjust(-5, -5, 5, 5);

    p.setBrush(Qt::darkGray);
    p.drawRoundedRect(rc2, 5, 5);

    p.drawText(rc, Qt::AlignCenter, m_label[i].label);
  }
}

void CHorizonWidget::mousePressEvent(QMouseEvent *e)
{
  if ((e->modifiers() & Qt::AltModifier) == Qt::AltModifier)
  {
    int x1;
    double y1;
    label_t label;

    getPos(e->pos(), x1, y1);

    label.azm = x1;
    label.label = m_currentText;

    m_label.append(label);

    updateData();
    update();
    return;
  }

  if ((e->modifiers() & Qt::ControlModifier) == Qt::ControlModifier)
  {
    int x1;
    double y1;

    getPos(e->pos(), x1, y1);

    m_map[x1] = y1;

    updateData();
    update();
    return;
  }

  if ((e->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier && m_map.count() > 2)
  {
    QMap<int, double>::iterator i;
    for (i = m_map.begin(); i != m_map.end(); i++)
    {
      QPointF pt(i.key(), i.value());

      double x1, y1;

      getPos(pt.x(), pt.y(), x1, y1);

      int deltaX = abs(x1 - e->x());
      int deltaY = abs(y1 - e->y());
      int dist = POW2(deltaX) + POW2(deltaY);

      if (dist <= POW2(4))
      {
        m_map.remove(i.key());

        updateData();
        update();
        return;
      }
    }
  }

  for (int i = 0; i < m_label.count(); i++)
  {
    int azm = m_label[i].azm;

    double x1, y1;
    QRect rc;

    getPos(azm, R2D(m_altHorizon[azm]) + 10, x1, y1);

    rc = QRect(-5, -5, 10, 10);
    rc.moveCenter(QPoint(x1, y1));

    if (rc.contains(e->pos()))
    {
      m_labelDrag = i;
      return;
    }
  }

  QMap<int, double>::iterator i;
  for (i = m_map.begin(); i != m_map.end(); i++)
  {
    QPointF pt(i.key(), i.value());

    double x1, y1;

    getPos(pt.x(), pt.y(), x1, y1);

    int deltaX = abs(x1 - e->x());
    int deltaY = abs(y1 - e->y());
    int dist = POW2(deltaX) + POW2(deltaY);

    if (dist <= POW2(4))
    {
      m_drag = true;
      m_index = i.key();


      if (i == m_map.begin())
      {
        m_left = 0;
      }
      else
      {
        QMap<int, double>::iterator itLeft = i - 1;
        m_left = itLeft.key() + 1;
      }

      if (i + 1 == m_map.end())
      {
        m_right = 359;
      }
      else
      {
        QMap<int, double>::iterator itRight = i + 1;
        m_right = itRight.key() - 1;
      }

      return;
    }
  }
}

void CHorizonWidget::mouseReleaseEvent(QMouseEvent *)
{
  m_drag = false;
  m_labelDrag = -1;
}

void CHorizonWidget::mouseMoveEvent(QMouseEvent *event)
{
  getPos(event->pos(), m_azm, m_alt);

  if (m_labelDrag >= 0)
  {
    m_azm = CLAMP(m_azm, 0, 359);
    m_label[m_labelDrag].azm = m_azm;
    updateData();
  }

  if (m_drag)
  {
    m_alt = CLAMP(m_alt, -90, 90);
    m_azm = CLAMP(m_azm, m_left, m_right);

    m_map.remove(m_index);
    m_map[m_azm] = m_alt;
    m_index = m_azm;
    updateData();
  }

  update();
}

void CHorizonWidget::updateData()
{
  m_list.clear();

  if (m_editor)
  {
    m_editor->clear();
    m_editor->appendPlainText("# Skytech horizon");
    m_editor->appendPlainText("# Azimuth, Horizon altitude");
    m_editor->appendPlainText("\n");
  }

  QMap<int, double>::iterator i;
  for (i = m_map.begin(); i != m_map.end(); i++)
  {
    QPointF pt(i.key(), i.value());

    if (m_editor)
    {
      m_editor->appendPlainText(QString("%1, %2").arg(i.key()).arg(i.value()));
    }

    m_list.append(pt);
  }

  if (m_editor)
  {
    m_editor->appendPlainText("\n");
    m_editor->appendPlainText("# Azimuth, Label above the horizon");
    m_editor->appendPlainText("\n");
    for (int i = 0; i < m_label.count(); i++)
    {
      m_editor->appendPlainText(QString("@%1, %2").arg(m_label[i].azm).arg(m_label[i].label));
    }
  }

  if (background.isTexture)
  {
    m_editor->appendPlainText("# Texture\n");
    m_editor->appendPlainText(QString("_texture = \"%1\";").arg(m_textureName));
  }

  background.makeHorizon(&m_list, m_altHorizon);
}

void CHorizonWidget::getPos(const QPoint &pos, int &azm, double &alt)
{
  double dx = 360 / (double)width();
  double dy = 180 / (double)height();

  azm = pos.x() * dx;
  alt = -((pos.y() * dy) - 90.0);
}

void CHorizonWidget::getPos(double azm, double alt, double &x, double &y)
{
  double dx = 360 / (double)width();
  double dy = 180 / (double)height();

  x = azm / dx;
  y = -(alt / dy) + (double)(height() / 2.0);
}
