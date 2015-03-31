#include "csatprediction.h"
#include "ui_csatprediction.h"
#include "csatellitedlg.h"

#include <QDebug>

CSatPrediction::CSatPrediction(QWidget *parent, mapView_t *view) :
  QDialog(parent),
  ui(new Ui::CSatPrediction)
{
  ui->setupUi(this);
  m_view = *view;

  fillList();

  ui->widget_2->setModel((QSortFilterProxyModel *)ui->listView_2->model(), 0);
  connect(ui->widget_2, SIGNAL(sigSetSelection(QModelIndex&)), this, SLOT(slotSelChange(QModelIndex&)));
}

CSatPrediction::~CSatPrediction()
{
  delete ui;
}

void CSatPrediction::fillList()
{
  QStandardItemModel *model = new QStandardItemModel(0, 1, NULL);
  model->setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));

  ui->listView_2->setModel(model);

  for (int i = 0; i < sgp4.count(); i++)
  {
    if (sgp4.tleItem(i)->used)
    {
      QStandardItem *item = new QStandardItem;

      item->setText(sgp4.getName(i));
      item->setData(i);
      item->setCheckable(true);
      item->setCheckState(Qt::Unchecked);
      item->setEditable(false);
      model->appendRow(item);
    }
  }
}


void CSatPrediction::slotSelChange(QModelIndex &index)
{
  ui->listView_2->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
  ui->listView_2->scrollTo(index);
}

void CSatPrediction::on_pushButton_clicked()
{
  QStandardItemModel *model = (QStandardItemModel *)ui->listView_2->model();

  for (int i = 0; i < model->rowCount(); i++)
  {
    if (model->item(i)->checkState() == Qt::Checked)
    {
      int index = model->item(i)->data().toInt();
      CSatPredictionSolver *solver;

      solver = new CSatPredictionSolver(index, 2457112.0, 2457112.35097222 + 20, &m_view);
      m_thread.append(solver);
      solver->start();
    }
  }

  if (m_thread.count() == 0)
  {
    msgBoxError(this, tr("Nothing selected!!!"));
    return;
  }

  qDebug() << m_thread.count();


}


CSatPredictionSolver::CSatPredictionSolver(int index, double fromJD, double toJD, mapView_t *view)
{
  qDebug() << "const";

  m_index = index;
  m_fromJD = fromJD;
  m_toJD = toJD;
  m_view = *view;
}

void CSatPredictionSolver::run()
{
  //std::list<struct PassDetails> pass_list;

      sgp4.setObserver(&m_view);
      satellite_t out;

      double aos_time;
      double los_time;

      bool found_aos = false;

      double previous_time = m_fromJD;
      double  current_time = m_fromJD;

      while (current_time < m_toJD)
      {
          bool end_of_pass = false;

          /*
           * calculate satellite position
           */
          //Eci eci = sgp4.FindPosition(current_time);
          //CoordTopocentric topo = obs.GetLookAngle(eci);
          m_view.jd = current_time;
          sgp4.solve(m_index, &m_view, &out);

          if (!found_aos && out.elevation > 0.0)
          {
              /*
               * aos hasnt occured yet, but the satellite is now above horizon
               * this must have occured within the last time_step
               */
              if (m_fromJD == current_time)
              {
                  /*
                   * satellite was already above the horizon at the start,
                   * so use the start time
                   */
                  aos_time = m_fromJD;
              }
              else
              {
                  /*
                   * find the point at which the satellite crossed the horizon
                   */
                  aos_time = findCrossingPoint(previous_time,
                                               current_time,
                                               true);
                  qDebug() << "a" << getStrDate(aos_time, 0) << getStrTime(aos_time, m_view.geo.tz);
              }
              found_aos = true;
          }
          else if (found_aos && out.elevation < 0.0)
          {
              found_aos = false;
              /*
               * end of pass, so move along more than time_step
               */
              end_of_pass = true;
              /*
               * already have the aos, but now the satellite is below the horizon,
               * so find the los
               */
              los_time = findCrossingPoint(previous_time,
                                           current_time,
                                           false);

              qDebug() << "b" << getStrDate(los_time, 0) << getStrTime(aos_time, m_view.geo.tz);

              /*
              struct PassDetails pd;
              pd.aos = aos_time;
              pd.los = los_time;
              */

              /*
              pd.max_elevation = FindMaxElevation(
                      user_geo,
                      sgp4,
                      aos_time,
                      los_time);
                      */

              //pass_list.push_back(pd);
          }

          /*
           * save current time
           */
          previous_time = current_time;

          if (end_of_pass)
          {
              /*
               * at the end of the pass move the time along by 30mins
               */
              current_time = current_time + 0.020833333;//  TimeSpan(0, 30, 0);
          }
          else
          {
              /*
               * move the time along by the time step value
               */
              current_time = current_time + JD1SEC * 10;//  180sec, TimeSpan(0, 0, time_step);
          }

          if (current_time > m_toJD)
          {
              /*
               * dont go past end time
               */
              current_time = m_toJD;
          }
      };

      if (found_aos)
      {
          /*
           * satellite still above horizon at end of search period, so use end
           * time as los
           */
        /*
          struct PassDetails pd;
          pd.aos = aos_time;
          pd.los = end_time;
          */
          //pd.max_elevation = FindMaxElevation(user_geo, sgp4, aos_time, end_time);

          //pass_list.push_back(pd);
      }


  qDebug() << "done";

}

double CSatPredictionSolver::findCrossingPoint(double initial_time1, double initial_time2, bool finding_aos)
{
     //Observer obs(user_geo);

     bool running;
     int cnt;
     satellite_t out;

     double time1 = initial_time1;
     double time2 = initial_time2;
     double middle_time;

     running = true;
     cnt = 0;
     while (running && cnt++ < 16)
     {
         //middle_time = time1 + 0.000011574 * ((time2 - time1).TotalSeconds() / 2.0);
          middle_time = time1 + JD1SEC * ((time2 - time1) / 2.0);
         /*
          * calculate satellite position
          */
         //Eci eci = sgp4.FindPosition(middle_time);
         //CoordTopocentric topo = obs.GetLookAngle(eci);
         m_view.jd = middle_time;
         sgp4.solve(m_index, &m_view, &out);

         if (out.elevation > 0.0)
         {
             /*
              * satellite above horizon
              */
             if (finding_aos)
             {
                 time2 = middle_time;
             }
             else
             {
                 time1 = middle_time;
             }
         }
         else
         {
             if (finding_aos)
             {
                 time1 = middle_time;
             }
             else
             {
                 time2 = middle_time;
             }
         }

         if ((time2 - time1) < 0.000011574)
         {
             /*
              * two times are within a second, stop
              */
             running = false;
             /*
              * remove microseconds
              */
             //int us = middle_time.Microsecond();
             //middle_time = middle_time.AddMicroseconds(-us);
             /*
              * step back into the pass by 1 second
              */
             //middle_time = middle_time.AddSeconds(finding_aos ? 1 : -1);
             middle_time += (finding_aos ? JD1SEC : -JD1SEC);
         }
     }

     /*
      * go back/forward 1second until below the horizon
      */
     running = true;
     cnt = 0;
     while (running && cnt++ < 6)
     {
         //Eci eci = sgp4.FindPosition(middle_time);
         //CoordTopocentric topo = obs.GetLookAngle(eci);
         m_view.jd = middle_time;
         sgp4.solve(m_index, &m_view, &out);

         if (out.elevation > 0)
         {
             //middle_time = middle_time.AddSeconds(finding_aos ? -1 : 1);
           middle_time += (finding_aos ? JD1SEC : -JD1SEC);
         }
         else
         {
             running = false;
         }
     }

     return middle_time;

}
