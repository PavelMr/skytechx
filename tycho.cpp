#include "tycho.h"
#include "jd.h"
#include "constellation.h"
#include "cgscreg.h"

CTycho cTYC;


static const char greekChar[25] = {'a','b','g','d','e','z','h','J','i','k',
                                   'l','m','n','x','o','p','r','s','t','u',
                                   'j','c','y','w','?'};



static const QString greekStr[25] = { "Alpha",
                                      "Beta",
                                      "Gamma",
                                      "Delta",
                                      "Epsilon",
                                      "Zeta",
                                      "Eta",
                                      "Theta",
                                      "Iota",
                                      "Kappa",
                                      "Lambda",
                                      "Mu",
                                      "Nu",
                                      "Xi",
                                      "Omicron",
                                      "Pi",
                                      "Rho",
                                      "Sigma",
                                      "Tau",
                                      "Upsilon",
                                      "Phi",
                                      "Chi",
                                      "Psi",
                                      "Omega",
                                      "???"
                                     };

typedef struct
{
  int reg;
  int no;
} hd_t;

static hd_t hdCross[360000];

////////////////
CTycho::CTycho()
////////////////
{
  for (int i = 0; i < (int)sizeof(hdCross) / (int)sizeof(hd_t); i++)
  {
    hdCross[i].reg = -1;
    hdCross[i].no = -1;
  }
}

///////////////////////////////////
QString CTycho::getGreekChar(int i)
///////////////////////////////////
{
  return(QString(greekChar[i]));
}

/////////////////////////////////////
QString CTycho::getGreekString(int i)
/////////////////////////////////////
{
  return(QString(greekStr[i]));
}

///////////////////
bool CTycho::load()
///////////////////
{
  SkFile f("../data/stars/tycho/tycho2.dat");

  if (!f.open(SkFile::ReadOnly))
  {
    // fatal error
    return false;
  }

  f.read((char *)&m_head, sizeof(m_head));

  if (m_head.id[0] != 'T' ||
      m_head.id[1] != 'Y' ||
      m_head.id[2] != 'C' ||
      m_head.id[3] != '2')
  {
    // fatal error
    return false;
  }

  m_region = (tychoRegion2_t *)malloc(m_head.regCount * sizeof(tychoRegion2_t));
  pSupplement = (tychoSupp_t *)malloc(m_head.numSupplements * sizeof(tychoSupp_t));

  qint64 pos = f.pos();

  f.seek(m_head.offSupplements);
  f.read((char *)pSupplement, m_head.numSupplements * sizeof(tychoSupp_t));

  f.seek(m_head.offNames);
  m_names = f.read(m_head.numNames);

  f.seek(pos);
  int maxhd = 0;

  qDebug() << "TSS" << sizeof(tychoStar_t) << sizeof(long);

  for (int i = 0; i < m_head.regCount; i++)
  {
    tychoRegion_t  reg;

    f.read((char *)&reg, sizeof(reg));

    cGSCReg.resetRegion();

    m_region[i].region.numStars = reg.numStars;
    m_region[i].stars = (tychoStar_t *)malloc(reg.numStars * sizeof(tychoStar_t));

    f.read((char *)m_region[i].stars, sizeof(tychoStar_t) * reg.numStars);

    for (int j = 0; j < reg.numStars; j++)
    {
      tychoStar_t *star = &m_region[i].stars[j];

      radec_t rd1;
      radec_t rd2;

      double yrMin = -2000;
      double yrMax = 1000;

      rd1.Ra = star->rd.Ra + (D2R(star->pmRa / 1000.0 / 3600.0) * yrMin * cos(star->rd.Dec));
      rd1.Dec = star->rd.Dec + D2R(star->pmDec / 1000.0 / 3600.0) * yrMin;

      rd2.Ra = star->rd.Ra + (D2R(star->pmRa / 1000.0 / 3600.0) * yrMax * cos(star->rd.Dec));
      rd2.Dec = star->rd.Dec + D2R(star->pmDec / 1000.0 / 3600.0) * yrMax;

      cGSCReg.addPoint(rd1);
      cGSCReg.addPoint(rd2);


      if (m_region[i].stars[j].supIndex >= 0)
      {
        int supp = m_region[i].stars[j].supIndex;
        int hd = pSupplement[supp].hd;

        if (hd >= 1 && hd <= 359083)
        { // max. HD je 359083
          hdCross[pSupplement[supp].hd].reg = i;
          hdCross[pSupplement[supp].hd].no = j;
        }
        else if (hd > maxhd) maxhd = hd;

        if (pSupplement[supp].pnOffs != 0xffff)
        {
          tNames.append(&m_region[i].stars[j]);
        }
      }
    }
    cGSCReg.createRegion(i);
  }

  cGSCReg.createOcTree();

  f.close();

  return(true);
}

//////////////////////////////////////////
tychoRegion2_t *CTycho::getRegion(int reg)
//////////////////////////////////////////
{
  return(&m_region[reg]);
}

//////////////////////////////////////////////
QString CTycho::getStarName(tychoSupp_t *supp)
//////////////////////////////////////////////
{
  QString str;

  if (supp->pnOffs == 0xffff)
    return("");

  for (int i = supp->pnOffs; ; i++)
  {
    char ch = m_names.at(i);
    if (ch == '\0')
      break;
    str += m_names.at(i);
  }

  return(str);
}


//////////////////////////////////////////////////////
bool CTycho::getStar(tychoStar_t **p, int reg, int no)
//////////////////////////////////////////////////////
{
  *p = &m_region[reg].stars[no];

  return(true);
}

/////////////////////////////////////////////
tychoStar_t *CTycho::getStar(int reg, int no)
/////////////////////////////////////////////
{
  return &m_region[reg].stars[no];
}


///////////////////////////////////////////////////////////
// return bayer letter + [number]
QString CTycho::getBayerStr(tychoSupp_t *supp, bool &found)
///////////////////////////////////////////////////////////
{
  if (supp->ba[0] == 0)
  {
    found = false;
    return("");
  }

  QString str = QString((greekChar[supp->ba[0] - 1]));

  if (supp->ba[1] != 0)
    str += supp->ba[1] + '0';

  found = true;
  return(str);
}

///////////////////////////////////////////////////////////////
// return bayer full string + [number]
QString CTycho::getBayerFullStr(tychoSupp_t *supp, bool &found)
///////////////////////////////////////////////////////////////
{
  if (supp->ba[0] == 0)
  {
    found = false;
    return("");
  }

  QString str = (greekStr[supp->ba[0] - 1]) + " ";

  if (supp->ba[1] != 0)
    str += supp->ba[1] + '0';

  found = true;
  return(str);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CTycho::findStar(QWidget *parent, int what, int flamsteed, int hd, int byLtr, int byNo, int tyc1, int tyc2, int tyc3, int constellation, int &reg, int &index)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  int from;
  int to;

  if (what == TS_TYC)
  {
    from = tyc1 - 1;
    to = tyc1;

    from = CLAMP(from, 0, m_head.regCount);
    to = CLAMP(to, 0, m_head.regCount);
  }
  else
  {
    from = 0;
    to = m_head.regCount;
  }

  QProgressDialog dlg(tr("Please wait..."), NULL, from, to, parent);

  if (parent)
  {
    dlg.setWindowFlags(((dlg.windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint));
    dlg.setWindowModality(Qt::WindowModal);
    dlg.show();
  }
  else
  {
    dlg.hide();
  }

  for (int i = from; i < to; i++)
  {
    tychoRegion2_t *tycReg = cTYC.getRegion(i);
    dlg.setValue(i);
    for (int j = 0; j < tycReg->region.numStars; j++)
    {
      tychoStar_t *s = &tycReg->stars[j];

      switch (what)
      {
        case TS_BAYER:
        {
          if (s->supIndex != -1)
          {
            tychoSupp_t *supp = &cTYC.pSupplement[s->supIndex];
            int con = constWhatConstel(s->rd.Ra, s->rd.Dec, JD2000);

            if (supp->ba[0] == byLtr && supp->ba[1] == byNo && con == constellation)
            {
              reg = i;
              index = j;
              return(true);
            }
          }
        }
        break;

        case TS_FLAMSTEED:
        {
          if (s->supIndex != -1)
          {
            tychoSupp_t *supp = &cTYC.pSupplement[s->supIndex];
            int con = constWhatConstel(s->rd.Ra, s->rd.Dec, JD2000);

            if (supp->fl == flamsteed && con == constellation)
            {
              reg = i;
              index = j;
              return(true);
            }
          }
        }
        break;

        case TS_TYC:
        {
          if (s->tyc1 == tyc1 &&
              s->tyc2 == tyc2 &&
              s->tyc3 == tyc3)
          {
            reg = i;
            index = j;
            return(true);
          }
        }
        break;

        case TS_HD:
          if (s->supIndex != -1)
          {
            tychoSupp_t *supp = &cTYC.pSupplement[s->supIndex];

            if (hd == supp->hd)
            {
              reg = i;
              index = j;
              return(true);
            }
          }
          break;
      }
    }
  }

  return(false);
}


///////////////////////////////////////
tychoStar_t *CTycho::findHDStar(int hd)
///////////////////////////////////////
{
  if (hd >= ((int)sizeof(hdCross) / (int)sizeof(hd_t)))
    return(NULL);

  hd_t h = hdCross[hd];

  if (h.reg == -1)
    return nullptr;

  tychoRegion2_t *tycReg = cTYC.getRegion(h.reg);

  return(&tycReg->stars[h.no]);
}

//////////////////////////////////////////
tychoStar_t *CTycho::findTYCStar(int *tyc)
//////////////////////////////////////////
{
  if (tyc[0] == 0 || tyc[0] > NUM_GSC_REGS)
    return(NULL);

  tychoRegion2_t *tycReg = cTYC.getRegion(tyc[0] - 1);

  for (int j = 0; j < tycReg->region.numStars; j++)
  {
    tychoStar_t *s = &tycReg->stars[j];

    if (s->tyc1 == tyc[0] &&
        s->tyc2 == tyc[1] &&
        s->tyc3 == tyc[2])
    {
      return(s);
    }
  }

  return(NULL);
}


///////////////////////////////////////////////////////////////
QString CTycho::getFlamsteedStr(tychoSupp_t *supp, bool &found)
///////////////////////////////////////////////////////////////
{
  if (supp->fl == 0)
  {
    found = false;
    return("");
  }

  found = true;
  return(QString::number(supp->fl));
}




