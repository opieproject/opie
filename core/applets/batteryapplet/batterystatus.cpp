
#include "batterystatus.h"

#include <qpe/power.h>

#include <opie/odevice.h>

#include <qpainter.h>
#include <qpushbutton.h>
#include <qdrawutil.h>
#include <qfile.h>
#include <qlayout.h>
#include <qtextstream.h>
#include <qapplication.h>
#include <qmessagebox.h>

using namespace Opie;

BatteryStatus::BatteryStatus( const PowerStatus *p, QWidget *parent )
  : QWidget( parent, 0, WDestructiveClose), ps(p), bat2(false) {
  setCaption( tr("Battery status") );
  QPushButton *pb = new QPushButton( tr("Close"), this );
  QVBoxLayout *layout = new QVBoxLayout ( this );

  jackPercent = 0;

  pb->setMaximumHeight(40);
  pb->setMaximumWidth( 120 );

  pb->show();

  layout->addStretch( 0 );
  layout->addWidget( pb );

  if ( ODevice::inst ( )-> series ( ) == Model_iPAQ ) {
      getProcApmStatusIpaq();
  }
  connect( pb, SIGNAL( clicked() ), this, SLOT( close() ) );
  percent = ps->batteryPercentRemaining();
  show();
}

BatteryStatus::~BatteryStatus()
{
}

/*
 * Make use of the advanced apm interface of the ipaq
 */
bool BatteryStatus::getProcApmStatusIpaq() {

  bat2 = false;

  QFile procApmIpaq("/proc/hal/battery");

  if (procApmIpaq.open(IO_ReadOnly) ) {
    QStringList list;
    // since it is /proc we _must_ use QTextStream
    QTextStream stream ( &procApmIpaq);
    QString streamIn;
    streamIn = stream.read();
    list = QStringList::split("\n", streamIn);

    for(QStringList::Iterator line=list.begin(); line!=list.end(); line++) {
      // not nice, need a rewrite later
      if( (*line).startsWith(" Percentage") ){
    	if (bat2 == true) {
	  perc2 = (*line).mid(((*line).find('('))+1,(*line).find(')')-(*line).find('(')-2);
	} else {
	  perc1 = (*line).mid(((*line).find('('))+1,(*line).find(')')-(*line).find('(')-2);
	}
      }else if( (*line).startsWith(" Life") ){
	if (bat2 == true) {
	  sec2 = (*line).mid(((*line).find(':')+2), 5 );
	} else {
	  sec1 = (*line).mid(((*line).find(':')+2), 5 );
	}
      }else if( (*line).startsWith("Battery #1") ){
	bat2 = true;
      }else if( (*line).startsWith(" Status") ){
	if (bat2 == true) {
	  jackStatus = (*line).mid((*line).find('(')+1., (*line).find(')')-(*line).find('(')-1);
        } else {
	  ipaqStatus = (*line).mid((*line).find('(')+1., (*line).find(')')-(*line).find('(')-1);
	}
      }else if( (*line).startsWith(" Chemistry") ) {
	if (bat2 == true) {
	  jackChem = (*line).mid((*line).find('('), (*line).find(')')-(*line).find('(')+1);
	} else {
	  ipaqChem = (*line).mid((*line).find('('), (*line).find(')')-(*line).find('(')+1);
	}
      }
    }
  } else {
    QMessageBox::warning(this, tr("Failure"),tr("could not open file"));
  }

  procApmIpaq.close();

  jackPercent = perc2.toInt();
  ipaqPercent = perc1.toInt();

  if (perc2.isEmpty()) {
    perc2 = "no data";
  } else {
    perc2 += " %";
  }


  if (sec2 == "0" || sec2 == "" || sec2.isEmpty()) {
    sec2 = "no data";
  } else {
    sec2 += " min";
  }

  jackStatus == (" ( " + jackStatus + " )");

  return true;
}


void BatteryStatus::updatePercent( int pc ) {
  percent = pc;
  repaint(FALSE);
}

void BatteryStatus::drawSegment( QPainter *p, const QRect &r, const QColor &topgrad, const QColor &botgrad, const QColor &highlight, int hightlight_height ) {
  int h1, h2, s1, s2, v1, v2, ng = r.height(), hy = ng*30/100, hh = hightlight_height;
  topgrad.hsv( &h1, &s1, &v1 );
  botgrad.hsv( &h2, &s2, &v2 );
  for ( int j = 0; j < hy-2; j++ ) {
    p->setPen( QColor( h1 + ((h2-h1)*j)/(ng-1), s1 + ((s2-s1)*j)/(ng-1),
		       v1 + ((v2-v1)*j)/(ng-1),  QColor::Hsv ) );
    p->drawLine( r.x(), r.top()+hy-2-j, r.x()+r.width(), r.top()+hy-2-j );
  }
  for ( int j = 0; j < hh; j++ ) {
    p->setPen( highlight );
    p->drawLine( r.x(), r.top()+hy-2+j, r.x()+r.width(), r.top()+hy-2+j );
    }
  for ( int j = 0; j < ng-hy-hh; j++ ) {
    p->setPen( QColor( h1 + ((h2-h1)*j)/(ng-1), s1 + ((s2-s1)*j)/(ng-1),
		       v1 + ((v2-v1)*j)/(ng-1),  QColor::Hsv ) );
    p->drawLine( r.x(), r.top()+hy+hh-2+j, r.x()+r.width(), r.top()+hy+hh-2+j );
  }
}

void BatteryStatus::paintEvent( QPaintEvent * ) {


    int screenWidth = qApp->desktop()->width();
    int screenHeight = qApp->desktop()->height();

  QPainter p(this);
  QString text;
  if ( ps->batteryStatus() == PowerStatus::Charging ) {
	if (bat2) {
	  text = tr("Charging both devices");
	} else {
	  text = tr("Charging");
	}
  } else if ( ps->batteryPercentAccurate() ) {
    text.sprintf( tr("Percentage battery remaining") + ": %i%%", percent );
  } else {
    text = tr("Battery status: ");
    switch ( ps->batteryStatus() ) {
    case PowerStatus::High:
      text += tr("Good");
      break;
    case PowerStatus::Low:
      text += tr("Low");
      break;
    case PowerStatus::VeryLow:
      text += tr("Very Low");
      break;
    case PowerStatus::Critical:
      text += tr("Critical");
      break;
    default: // NotPresent, etc.
      text += tr("Unknown");
    }
  }
  p.drawText( 10, 90, text );

  if ( ps->acStatus() == PowerStatus::Backup )
    p.drawText( 10, 110, tr("On backup power") );
  else if ( ps->acStatus() == PowerStatus::Online )
    p.drawText( 10, 110, tr("Power on-line") );
  else if ( ps->acStatus() == PowerStatus::Offline )
    p.drawText( 10, 110, tr("External power disconnected") );

  if ( ps->batteryTimeRemaining() >= 0 ) {
    text.sprintf( tr("Battery time remaining") + ": %im %02is",
		  ps->batteryTimeRemaining() / 60, ps->batteryTimeRemaining() % 60 );
    p.drawText( 10, 130, text );
  }

  QColor c;
  QColor darkc;
  QColor lightc;
  if ( ps->acStatus() == PowerStatus::Offline ) {
    c = blue.light(120);
    darkc = c.dark(280);
    lightc = c.light(145);
  } else if ( ps->acStatus() == PowerStatus::Online ) {
    c = green.dark(130);
    darkc = c.dark(200);
    lightc = c.light(220);
  } else {
    c = red;
    darkc = c.dark(280);
    lightc = c.light(140);
  }
  if ( percent < 0 )
    return;

  int rightEnd1 = screenWidth - 47;
  int rightEnd2 = screenWidth - 35;
  int percent2 = ( percent  / 100 ) * rightEnd1 ;
  p.setPen( black );
  qDrawShadePanel( &p,   9, 30, rightEnd1 , 39, colorGroup(), TRUE, 1, NULL);
  qDrawShadePanel( &p, rightEnd2, 37,  12, 24, colorGroup(), TRUE, 1, NULL);
  drawSegment( &p, QRect( 10, 30, percent2, 40 ), lightc, darkc, lightc.light(115), 6 );
  drawSegment( &p, QRect( 11 + percent2, 30,  rightEnd1 - percent2, 40 ), white.light(80), black, white.light(90), 6 );
  drawSegment( &p, QRect( rightEnd2, 37, 10, 25 ), white.light(80), black, white.light(90), 2 );
  p.setPen( black);


  if ( ODevice::inst ( )-> series ( ) == Model_iPAQ ) {

      p.drawText(15, 50, tr ("Ipaq  " + ipaqChem));

      QString jacketMsg;
      if (bat2) {
          p.setPen(black);
          p.drawText(10,220, tr("Percentage battery remaining: ") + perc2 + " " + jackStatus);
          p.drawText(10,240, tr("Battery time remaining: ") + sec2);
          jacketMsg = tr("Jacket  " + jackChem);
      } else {
          jackPercent = 0;
          jacketMsg = tr("No jacket with battery inserted");
      }

      int jackPerc =  ( jackPercent / 100 ) * screenWidth - 47;

      qDrawShadePanel( &p,   9, 160, rightEnd1, 39, colorGroup(), TRUE, 1, NULL);
      qDrawShadePanel( &p, rightEnd2, 167,  12, 24, colorGroup(), TRUE, 1, NULL);
      drawSegment( &p, QRect( 10, 160, jackPerc, 40 ), lightc, darkc, lightc.light(115), 6 );
      drawSegment( &p, QRect( 11 + jackPerc, 160, rightEnd1 - jackPerc, 40 ), white.light(80), black, white.light(90), 6 );
      drawSegment( &p, QRect( rightEnd2, 167, 10, 25 ), white.light(80), black, white.light(90), 2 );
      p.setPen( black );
      p.drawText(15, 180, jacketMsg);
  }

}

