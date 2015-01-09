#include "csearchwidget.h"
#include "ui_csearchwidget.h"

#include <QDebug>

CSearchWidget::CSearchWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::CSearchWidget)
{
  ui->setupUi(this);
}

CSearchWidget::~CSearchWidget()
{
  delete ui;
}

void CSearchWidget::setModel(QSortFilterProxyModel *model, int column)
{
  m_model = model;
  m_column = column;
}

void CSearchWidget::on_lineEdit_textChanged(const QString &arg1)
{
  m_row = 0;
  m_text = arg1;

  if (m_text.isEmpty())
  {
    return;
  }

  m_row = findItem(arg1, 0);
  if (m_row != -1)
  {
    QModelIndex index = m_model->index(m_row, m_column);
    emit sigSetSelection(index);
  }
}

int CSearchWidget::findItem(const QString &text, int fromRow)
{
  for (int i = fromRow; i < m_model->rowCount(); i++)
  {
    QModelIndex index = m_model->index(i, m_column);
    QString itemText = index.data().toString();

    if (itemText.contains(text, Qt::CaseInsensitive))
    {
      return i;
    }
  }

  return -1;
}

void CSearchWidget::on_pb_search_clicked()
{
  if (m_text.isEmpty())
  {
    return;
  }

  m_row++;
  m_row = findItem(m_text, m_row);
  if (m_row != -1)
  {
    QModelIndex index = m_model->index(m_row, m_column);
    emit sigSetSelection(index);
  }
  else
  {
    m_row = 0;
  }
}
