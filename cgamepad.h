#ifndef CGAMEPAD_H
#define CGAMEPAD_H

#include <QObject>
#include <QTimer>

#include <windows.h>

// button 0-31
#define GP_X_PLUS_AXIS     100
#define GP_X_MINUS_AXIS    101
#define GP_Y_PLUS_AXIS     102
#define GP_Y_MINUS_AXIS    103
#define GP_Z_PLUS_AXIS     104
#define GP_Z_MINUS_AXIS    105
#define GP_POV_LEFT        110
#define GP_POV_RIGHT       111
#define GP_POV_UP          112
#define GP_POV_DOWN        113
#define GP_R_PLUS_AXIS     114
#define GP_R_MINUS_AXIS    115

#define SKC_MOVE_LEFT        0
#define SKC_MOVE_RIGHT       1
#define SKC_MOVE_UP          2
#define SKC_MOVE_DOWN        3
#define SKC_ZOOM_IN          4
#define SKC_ZOOM_OUT         5

#define SKC_STAR_MAG_PLUS    6
#define SKC_STAR_MAG_MINUS   7
#define SKC_DSO_MAG_PLUS     8
#define SKC_DSO_MAG_MINUS    9

typedef struct
{
  int gamepad;            // GP_xxx
  int skytechControl;     // SKC_
} gamepadControl_t;

typedef struct
{
  QList<gamepadControl_t> config;
  int                     device;
  int                     period;
  bool                    used;
  double                  deadZone;
  double                  speedMul;
} gamepadConfig_t;

typedef struct
{
  double starMagPlus;
  double starMagMinus;
  double DSOMagPlus;
  double DSOMagMinus;
  double left;
  double right;
  double up;
  double down;
  double zoomIn;
  double zoomOut;
} gamepad_t;

typedef struct
{
  QString name;
  int     numButtons;
  int     numAxis;
  bool    isPOV;
} gamePadInfo_t;

class CGamepad : public QObject
{
  Q_OBJECT
public:
  explicit CGamepad(int period = 50, double deadZone = 0.1, double speedMul = 1.0, QObject *parent = 0);
  ~CGamepad();

  static bool getDeviceInfo(int devId, gamePadInfo_t *info);

  bool configure(int devId, QList <gamepadControl_t> &list);

private:
  QTimer *m_timer;

  QList <gamepadControl_t> m_list;

  int m_devId;

  double m_deadZone;
  double m_speedMul;

  int m_xMinMax[2];
  int m_yMinMax[2];
  int m_zMinMax[2];
  int m_rMinMax[2];

  bool m_ok;

  double readData(int axis, JOYINFOEX * joy);

signals:

  void sigChanged(const gamepad_t &state, double mul);

public slots:
  void slotTimeout();

};

#endif // CGAMEPAD_H
