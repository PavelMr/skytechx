#include <stdio.h>
#include <math.h>

#include "QtCore"
#include "skcore.h"

/////////////////////////////////////////////////
void getHO2000(QString &str,double Ra,double Dec)
/////////////////////////////////////////////////
{
  double D = RAD2DEG(Dec);
  double R = RAD2DEG(Ra) / 15.0;
  double r = R - 2;
  int pg;

  if (D >= 60)
  {
    pg = 1;
    str = QString(QObject::tr("Pg. %1")).arg(pg);
    return;
  }

  if (D <= -60)
  {
    pg = 18;
    str = QString(QObject::tr("Pg. %1")).arg(pg);
    return;
  }

  if (D >= 0)
  {
    pg = (int)(3 + (r / 3));
    if (R >= 22) pg = 2;
    str = QString(QObject::tr("Pg. %1")).arg(pg);
    return;
  } else
  {
    pg = (int)(11 + (r / 3));
    if (R >= 22) pg = 10;
  }

  str = QString(QObject::tr("Pg. %1")).arg(pg);
}


/////////////////////////////////////////////////////////////
void getMilleniumStasAtlas(QString &str,double Ra,double Dec)
/////////////////////////////////////////////////////////////
{
  double D = RAD2DEG(Dec);
  double H = RAD2DEG(Ra) / 15.0;
  int QTE,PAS,QTEN,VOL,CALA,HM,CART;
  int ds = 0;
  char vstr[5][6] = {"I","II","III","IV","V"};

  VOL = 3;

  if (Dec < 0) ds = 1;

  HM = (int)H;

  if (fabs(D) > 87) H = 0;

  if (H >= 0 && H <= 8) VOL = 0;
  if (H >  8 && H <= 16) VOL = 1;
  if (H > 16) VOL = 2;

  QTE=0;
  PAS=0;
  QTEN=0;
  if (fabs(D)>90) D=90;
  if (fabs(D)<=90) { PAS=240; QTE=QTE+2; QTEN=2; }
  if (fabs(D)<87) {PAS=120; QTE=QTE+4; QTEN=4; }
  if (fabs(D)<81) {PAS=60; QTE=QTE+8; QTEN=8;  }
  if (fabs(D)<75) {PAS=48; QTE=QTE+10; QTEN=10; }
  if (fabs(D)<69) {PAS=40; QTE=QTE+12; QTEN=12; }
  if (fabs(D)<63) {PAS=480/14; QTE=QTE+14; QTEN=14;}
  if (fabs(D)<57) {PAS=30; QTE=QTE+16; QTEN=16;}
  if (fabs(D)<51) {PAS=24; QTE=QTE+20; QTEN=20; }
  if (fabs(D)<45) {PAS=24; QTE=QTE+20; QTEN=20;  }
  if (fabs(D)<39) {PAS=480/22; QTE=QTE+22; QTEN=22;}
  if (fabs(D)<33) {PAS=480/22; QTE=QTE+22; QTEN=22;}
  if (fabs(D)<27) {PAS=20; QTE=QTE+24; QTEN=24;}
  if (fabs(D)<21) {PAS=20; QTE=QTE+24; QTEN=24;}
  if (fabs(D)<15) {PAS=20; QTE=QTE+24; QTEN=24;}
  if (fabs(D)<9) {PAS=20; QTE=QTE+24; QTEN=24;}
  if (fabs(D)<3) {PAS=20; QTE=QTE+24; QTEN=24;}

  if (H==8) H = 7.99;
  if (H==16) H=15.99;
  if (H==24) H=23.99;

  if (H > VOL * 8) H = H - (VOL*8);
  CALA=(int)((H*60)/PAS);
  if (fabs(D)>87 && (HM>4 && HM<16)) { QTE=1; QTEN=0;}
  CART=QTE-CALA+(VOL*516);
  if (ds) CART = (516+(VOL*516)-QTE+QTEN-CALA);

  str = QString(QObject::tr("Pg. %1-%2, Vol. %3")).arg(CART).arg(CART + 1).arg(vstr[VOL]);
}



////////////////////////////////////////////////////////
void getStarAtlas2000(QString &str,double Ra,double Dec)
////////////////////////////////////////////////////////
{
  double D = RAD2DEG(Dec);
  double H = RAD2DEG(Ra) / 15.0;
  int S;

  if (fabs(D) >= 18.5) goto p370;
  S = 9 + (int)(H / 3 + 1 / 1.2);
  if (S == 9)	S = 17;
  goto p460;
 p370:
  if (fabs(D) >= 52) goto p410;
  S = 4 + (int)(H / 4);
  if (D < 0) S = S + 14;
  goto p460;
 p410:
  S = 1 + (int)(H / 8);
  if(D < 0) S = S + 23;
 p460:

 str = QString(QObject::tr("Pg. %1")).arg(S);
}


///////////////////////////////////////////////////////////////
void getUranometriaAtlas2000(QString &str,double Ra,double Dec)
///////////////////////////////////////////////////////////////
{
  double D = RAD2DEG(Dec);
  double H = RAD2DEG(Ra) / 15.0;
  int U = 0;
  char vstr[8] = "?";

  if (fabs(D)>=5.5) goto	p510;
  U=215+(int)(H*1.875+.5);
  if ( U==260) U=215;
  strcpy(vstr, "I & II");
  goto p900;
 p510:
  if ( D>0) 	strcpy(vstr,"I");
  if ( D<0) 	strcpy(vstr,"II");
  if ( fabs(D)>=50) goto p730;
  if ( fabs(D)>=28) goto p640;
  if ( fabs(D)>=17) goto p600;
  U=170+(int)(H*1.875+.5);
  if ( U==215) U=170;
  if ( D<0) U=U+90;
  goto p900;
 p600:
  U=125+(int)(H*1.875+0.5);
  if (U==170) U = 125;
  if (D<0) U += 180;
  goto p900;
 p640:
  if (fabs(D)>=39) goto p690;
  U=89+(int)(H*1.5+.5);
  if ( U==125) U=89;
  if ( D<0) U=U+261;
  goto p900;
 p690:
  U=59+(int)(H/.8+.5);
  if ( U==89) U=59;
  if ( D<0) U=U+327;
  goto p900;
 p730:
  if ( fabs(D)>=72.5) goto p830;
  if (fabs(D)>=61) goto p790;
  U=35+(int)(H+.5);
  if ( U==59) U=35;
  if ( D<0) U=U+381;
  goto p900;
 p790:
  U=15+(int)(H/1.2+.5);
  if ( U==35) U=15;
  if ( D<0) U=U+425;
  goto p900;
 p830:
  if (fabs(D)>=84.5) goto p880;
  U=3+(int)(H/2+1/2.4);
  if (U==15) U=3;
  if (D<0) U=U+457;
  goto p900;
 p880:
  U=1+(int)(H/12);
  if ( D<0)	U=474-U;
 p900:

  str = QString(QObject::tr("Pg. %1, Vol %2")).arg(U).arg(vstr);
}

