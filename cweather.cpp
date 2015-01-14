#include "cweather.h"
#include "ui_cweather.h"
#include "skcore.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QJsonArray>
#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>

#define M_KELVIN      273.15
#define C_TO_F(c)     (((9 * (c)) / 5.0) + 32.0)
#define MS_TO_MPH(ms) (ms * 2.23693629)

//static QString json = "{\"coord\":{\"lon\":139,\"lat\":35},\"sys\":{\"type\":3,\"id\":10354,\"message\":0.1654,\"country\":\"JP\",\"sunrise\":1409170427,\"sunset\":1409217397},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"sky is clear\",\"icon\":\"02n\"}],\"base\":\"cmc stations\",\"main\":{\"temp\":295.22,\"humidity\":96,\"pressure\":1013,\"temp_min\":294.26,\"temp_max\":297.04},\"wind\":{\"speed\":2.57,\"gust\":3.08,\"deg\":1},\"clouds\":{\"all\":8},\"dt\":1409233155,\"id\":1851632,\"name\":\"Shuzenji\",\"cod\":200}";

CWeather::CWeather(QWidget *parent, mapView_t *view) :
  QDialog(parent),
  ui(new Ui::CWeather)
{
  ui->setupUi(this);
  setWindowTitle(windowTitle() + " - openweathermap.org");

  connect(&m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotDownloadFinished(QNetworkReply*)));
  m_lon = R2D(view->geo.lon);
  m_lat = R2D(view->geo.lat);
  getData();
}

CWeather::~CWeather()
{
  delete ui;
}

void CWeather::updateInfo()
{
  QJsonDocument doc = QJsonDocument::fromJson(m_json.toUtf8());
  QJsonObject obj = doc.object();
  QJsonObject tempObject;
  QJsonValue val;
  double temp = 0;
  double humidity = 0;
  double pressure = 0;
  double windSpeed = 0;
  double windDeg = 0;
  double cloudiness = 0;
  QString icon;
  QString cityName;
  QString country = "N/A";
  QDateTime dateTime;

  if (obj.contains(QStringLiteral("main")))
  {
    val = obj.value(QStringLiteral("main"));
    tempObject = val.toObject();
    temp = tempObject.value(QStringLiteral("temp")).toDouble();
    humidity = tempObject.value(QStringLiteral("humidity")).toDouble();
    pressure = tempObject.value(QStringLiteral("pressure")).toDouble();
  }


  if (obj.contains(QStringLiteral("weather")))
  {
    val = obj.value(QStringLiteral("weather"));

    QJsonArray weatherArray = val.toArray();

    val = weatherArray.at(0);
    tempObject = val.toObject();
    icon = tempObject.value("icon").toString() + ".png";
  }

  if (obj.contains(QStringLiteral("sys")))
  {
    val = obj.value(QStringLiteral("sys"));
    tempObject = val.toObject();
    country = tempObject.value(QStringLiteral("country")).toString();
  }

  if (obj.contains(QStringLiteral("name")))
  {
    val = obj.value(QStringLiteral("name"));
    cityName = val.toString();
  }

  if (obj.contains(QStringLiteral("dt")))
  {
    val = obj.value(QStringLiteral("dt"));
    dateTime = QDateTime::fromTime_t(val.toInt());
  }

  if (obj.contains(QStringLiteral("wind")))
  {
    val = obj.value(QStringLiteral("wind"));
    tempObject = val.toObject();
    windSpeed = tempObject.value(QStringLiteral("speed")).toDouble();
    windDeg = tempObject.value(QStringLiteral("deg")).toDouble();
  }

  if (obj.contains(QStringLiteral("clouds")))
  {
    val = obj.value(QStringLiteral("clouds"));
    tempObject = val.toObject();
    cloudiness = tempObject.value(QStringLiteral("all")).toDouble();
  }

  if (m_error.isEmpty())
  {
    QLocale locale;

    ui->label_1->setText(tr("Temperature : ") + QString("%1°C / %2°F").arg((temp - M_KELVIN), 0, 'f', 1).arg(C_TO_F(temp - M_KELVIN), 0, 'f', 1));
    ui->label_2->setText(tr("Humidity : ") + QString("%1%").arg(humidity));
    ui->label_3->setText(tr("Pressure : ") + QString("%1 hPa").arg(pressure, 0, 'f', 0));
    ui->label_4->setText(tr("Wind speed : ") + QString("%1 m/s / %2 mph").arg(windSpeed, 0, 'f', 1).arg(MS_TO_MPH(windSpeed), 0, 'f', 1));
    ui->label_5->setText(tr("Wind dir. : ") + QString("%1°").arg(windDeg, 0, 'f', 0));
    ui->label_6->setText(tr("Clouds : ") + QString("%1%").arg(cloudiness, 0, 'f', 0));
    ui->label_7->setText(tr("Name : ") + cityName + ", " + country);
    ui->label_8->setText(tr("Date/Time : ") + locale.toString(dateTime, QLocale::ShortFormat));
  }
  else
  {
    ui->label_1->setText("");
    ui->label_2->setText("");
    ui->label_3->setText("");
    ui->label_4->setText("");
    ui->label_5->setText("");
    ui->label_6->setText("");
    ui->label_7->setText("");
    ui->label_8->setText("");
  }

  qDebug() << (temp - M_KELVIN) << C_TO_F(temp - M_KELVIN);
  qDebug() << humidity;
  qDebug() << pressure;
  qDebug() << windSpeed << MS_TO_MPH(windSpeed);
  qDebug() << windDeg;
  qDebug() << cloudiness;
  qDebug() << icon;

  ui->label_icon->setPixmap(":/res/weather/" + icon);

}

void CWeather::getData()
{
  //QUrl qurl("http://localhost/json/data.json");
  QUrl qurl(QString("http://api.openweathermap.org/data/2.5/weather?lat=%1&lon=%2&mode=json").arg(m_lat).arg(m_lon));

  QNetworkRequest request(qurl);
  QNetworkReply *reply = m_manager.get(request);

  ui->pushButton->setEnabled(false);

  Q_UNUSED(reply);
}

void CWeather::slotDownloadFinished(QNetworkReply *reply)
{
  m_json = "";

  ui->pushButton->setEnabled(true);
  qDebug() << "error" << reply->error();

  if (reply->error() == QNetworkReply::NoError)
  {
    m_json = reply->readAll();
    m_error = "";
  }
  else
  {
    m_error = reply->errorString();
    msgBoxError(this, m_error);
  }

  qDebug() << m_json;
  updateInfo();

  reply->deleteLater();
}

void CWeather::on_pushButton_2_clicked()
{
  done(DL_OK);
}

void CWeather::on_pushButton_clicked()
{
  getData();
}
