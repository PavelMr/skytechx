#include "cgamepad.h"
#include "skcore.h"

#ifdef Q_OS_WIN32

CGamepad::CGamepad(int period, double deadZone, double speedMul, QObject *parent) :
  QObject(parent) ,
  m_deadZone(deadZone),
  m_speedMul(speedMul),
  m_ok(false)
{
  m_timer = new QTimer(this);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));
  m_timer->start(period);
}

CGamepad::~CGamepad()
{
}

bool CGamepad::getDeviceInfo(int devId, gamePadInfo_t *info)
{
  JOYCAPS caps;

  MMRESULT res = joyGetDevCaps(devId, &caps, sizeof(caps));
  if (res != JOYERR_NOERROR)
  {
    return false;
  }

  info->isPOV = caps.wCaps & JOYCAPS_HASPOV;
  info->numAxis = caps.wNumAxes;
  info->numButtons = caps.wMaxButtons;
  info->name = QString::fromStdWString(caps.szPname);

  return true;
}

bool CGamepad::configure(int devId, QList<gamepadControl_t> &list)
{
  m_devId = devId;

  m_list = list;

  JOYCAPS caps;

  MMRESULT res = joyGetDevCaps(devId, &caps, sizeof(caps));
  if (res != JOYERR_NOERROR)
  {
    qDebug() << "error" << m_devId;
    return false;
  }

  m_xMinMax[0] = caps.wXmin;
  m_xMinMax[1] = caps.wXmax;

  m_yMinMax[0] = caps.wYmin;
  m_yMinMax[1] = caps.wYmax;

  m_zMinMax[0] = caps.wZmin;
  m_zMinMax[1] = caps.wZmax;

  m_rMinMax[0] = caps.wRmin;
  m_rMinMax[1] = caps.wRmax;

  m_ok = true;
  return true;
}

double CGamepad::readData(int axis, JOYINFOEX *joy)
{
  int value;
  double dVal;
  double minVal;
  double maxVal;
  double pov = joy->dwPOV / 100.0;  

  switch (axis)
  {
    case GP_POV_LEFT:
    case GP_POV_RIGHT:
    case GP_POV_UP:
    case GP_POV_DOWN:
      if (pov >= 360)
      {
        return 0.0;
      }
      break;
  }

  switch (axis)
  {
    case GP_POV_LEFT:
      return (pov > 180) ? qAbs(sin(D2R(pov))) : 0;

    case GP_POV_RIGHT:
      return (pov < 180) ? qAbs(sin(D2R(pov))) : 0;

    case GP_POV_UP:
      return (pov > 270 || pov < 90) ? qAbs(cos(D2R(pov))) : 0;

    case GP_POV_DOWN:
      return (pov > 90 && pov < 270) ? qAbs(cos(D2R(pov))) : 0;

    case GP_X_PLUS_AXIS:
    case GP_X_MINUS_AXIS:
      value = joy->dwXpos;
      minVal = m_xMinMax[0];
      maxVal = m_xMinMax[1];
      break;

    case GP_Y_PLUS_AXIS:
    case GP_Y_MINUS_AXIS:
      value = joy->dwYpos;
      minVal = m_yMinMax[0];
      maxVal = m_yMinMax[1];      
      break;

    case GP_Z_PLUS_AXIS:
    case GP_Z_MINUS_AXIS:
      value = joy->dwZpos;
      minVal = m_zMinMax[0];
      maxVal = m_zMinMax[1];
      break;

    case GP_R_PLUS_AXIS:
    case GP_R_MINUS_AXIS:
      value = joy->dwRpos;
      minVal = m_rMinMax[0];
      maxVal = m_rMinMax[1];
      break;

    default:
      return (joy->dwButtons & (1 << axis)) ? 1.0 : 0.0;
  }

  double delta = maxVal - minVal;
  dVal = ((value / delta) * 2) - 1;
  dVal = CLAMP(dVal, -1, 1);

  if (dVal >= -m_deadZone && dVal <= m_deadZone)
  {
    dVal = 0;
  }  

  switch (axis)
  {
    case GP_X_PLUS_AXIS:
    case GP_Y_PLUS_AXIS:
    case GP_Z_PLUS_AXIS:
    case GP_R_PLUS_AXIS:
      dVal = CLAMP(dVal, 0, 1);
      break;

    case GP_X_MINUS_AXIS:
    case GP_Y_MINUS_AXIS:
    case GP_Z_MINUS_AXIS:
    case GP_R_MINUS_AXIS:
      dVal = CLAMP(dVal, -1, 0);
      break;
  }

  QEasingCurve curve = QEasingCurve(QEasingCurve::InCirc);

  dVal = curve.valueForProgress(qAbs(dVal));  

  return dVal;
}

void CGamepad::slotTimeout()
{
  if (!m_ok)
  {
    return;
  }

  JOYINFOEX joy;

  joy.dwFlags = JOY_RETURNALL;
  joy.dwSize = sizeof(joy);

  MMRESULT res = joyGetPosEx(m_devId, &joy);
  if (res != JOYERR_NOERROR)
  {
    return;
  }

  gamepad_t state;

  memset(&state, 0, sizeof(state));

  foreach (const gamepadControl_t &item, m_list)
  {
    switch (item.skytechControl)
    {
      case SKC_STAR_MAG_PLUS:
        if (state.starMagPlus <= 0)
        {
          state.starMagPlus = readData(item.gamepad, &joy);
        }
        break;

      case SKC_STAR_MAG_MINUS:
        if (state.starMagMinus <= 0)
        {
          state.starMagMinus = readData(item.gamepad, &joy);
        }
        break;

      case SKC_DSO_MAG_PLUS:
        if (state.DSOMagPlus <= 0)
        {
          state.DSOMagPlus = readData(item.gamepad, &joy);
        }
        break;

      case SKC_DSO_MAG_MINUS:
        if (state.DSOMagMinus <= 0)
        {
          state.DSOMagMinus = readData(item.gamepad, &joy);
        }
        break;

      case SKC_MOVE_LEFT:
        if (state.left <= 0)
        {
          state.left = readData(item.gamepad, &joy);
        }
        break;

      case SKC_MOVE_RIGHT:
        if (state.right <= 0)
        {
          state.right = readData(item.gamepad, &joy);
        }
        break;

      case SKC_MOVE_UP:
        if (state.up <= 0)
        {
          state.up = readData(item.gamepad, &joy);
        }
        break;

      case SKC_MOVE_DOWN:
        if (state.down <= 0)
        {
          state.down = readData(item.gamepad, &joy);
        }
        break;

      case SKC_ZOOM_IN:
        if (state.zoomIn <= 0)
        {
          state.zoomIn = readData(item.gamepad, &joy);
        }
        break;

      case SKC_ZOOM_OUT:
        if (state.zoomOut <= 0)
        {
          state.zoomOut = readData(item.gamepad, &joy);
        }
        break;
    }
  }

  if (state.down > 0 ||
      state.up > 0 ||
      state.left > 0 ||
      state.right > 0 ||
      state.zoomIn > 0 ||
      state.zoomOut > 0 ||
      state.starMagPlus > 0 ||
      state.starMagMinus > 0 ||
      state.DSOMagPlus > 0 ||
      state.DSOMagMinus > 0)
  {
    sigChanged(state, m_speedMul);
  }
}


#endif // Q_OS_WIN32
