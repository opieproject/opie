
#include "batterystatus.h"

/* OPIE */
#include <opie2/odevice.h>
#include <qpe/power.h>

/* QT */
#include <qpushbutton.h>
#include <qdrawutil.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>

using namespace Opie::Core;

BatteryStatus::BatteryStatus( const PowerStatus *p, QWidget *parent, WFlags f )
: QFrame( parent, 0, f), ps(p), bat2(false) {

    jackPercent = 0;

    if ( ODevice::inst ( )-> series ( ) == Model_iPAQ ) {
        getProcApmStatusIpaq();
    }
    percent = ps->batteryPercentRemaining();
}

BatteryStatus::~BatteryStatus() {}

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
            if( (*line).startsWith(" Percentage") ) {
                if (bat2 == true) {
                    perc2 = (*line).mid(((*line).find('(')) +1,(*line).find(')')-(*line).find('(')-2);
                } else {
                    perc1 = (*line).mid(((*line).find('('))+1,(*line).find(')')-(*line).find('(')-2);
                }
            } else if( (*line).startsWith(" Life") ) {
                if (bat2 == true) {
                    sec2 = (*line).mid(((*line).find(':')+2), 5 );
                } else {
                    sec1 = (*line).mid(((*line).find(':')+2), 5 );
                }
            } else if( (*line).startsWith("Battery #1") ) {
                bat2 = true;
            } else if( (*line).startsWith(" Status") ) {
                if (bat2 == true) {
                    jackStatus = (*line).mid((*line).find('(')+1, (*line).find(')')-(*line).find('(')-1);
                } else {
                    ipaqStatus = (*line).mid((*line).find('(')+1, (*line).find(')')-(*line).find('(')-1);
                }
            } else if( (*line).startsWith(" Chemistry") ) {
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
        perc2 = tr("no data");
    } else {
        perc2 += " %";
    }

    if (sec2 == "0" || sec2 == "" || sec2.isEmpty()) {
        sec2 = tr("no data");
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

QString  BatteryStatus::statusText() const {
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

    if ( ps->acStatus() == PowerStatus::Backup )
    text +=  "\n"  +  tr("On backup power");
    else if ( ps->acStatus() == PowerStatus::Online )
        text +=  "\n"  + tr("Power on-line");
        else if ( ps->acStatus() == PowerStatus::Offline )
            text +=  "\n"  +  tr("External power disconnected");

            if ( ps->batteryTimeRemaining() >= 0 ) {
                text += "\n" + QString().sprintf(  tr("Battery time remaining") + ": %im %02is",
                                                       ps->batteryTimeRemaining() / 60, ps->batteryTimeRemaining() % 60 );
                }
    return text;
}

QString  BatteryStatus::statusTextIpaq() const {
    QString text;
    text +=  tr("Percentage battery remaining: ") + perc2 + " " + jackStatus;
    text += "\n"  + tr("Battery time remaining: ") + sec2;
    return text;
}

void BatteryStatus::paintEvent( QPaintEvent * ) {

    QPainter p( this );

    QString text = statusText();
    p.drawText( 10, 50, width() - 20, 40 , AlignVCenter, text );

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

    int rightEnd1 = width() - 47;
    int rightEnd2 = width() - 35;
    int percent2 = (percent * rightEnd1) / 100;
    p.setPen( black );
    qDrawShadePanel( &p,   9, 10, rightEnd1 , 39, colorGroup(), TRUE, 1, NULL);
    qDrawShadePanel( &p, rightEnd2, 17,  12, 24, colorGroup(), TRUE, 1, NULL);
    drawSegment( &p, QRect( 10, 10, percent2, 40 ), lightc, darkc, lightc.light(115), 6 );
    drawSegment( &p, QRect( 11 + percent2, 10,  rightEnd1 - percent2, 40 ), white.light(80), black, white.light(90), 6 );
    drawSegment( &p, QRect( rightEnd2, 17, 10, 25 ), white.light(80), black, white.light(90), 2 );
    p.setPen( black);

    if ( ODevice::inst ( )-> series ( ) == Model_iPAQ && bat2 ) {

        p.drawText( 15, 30, tr ("Ipaq  ") + ipaqChem );

        QString jacketMsg;
        if (bat2) {
            p.setPen(black);
            QString text = statusTextIpaq();
            p.drawText( 10, 150,  text );
            jacketMsg = tr("Jacket  ") + jackChem;
        } else {
            jackPercent = 0;
            jacketMsg = tr("No jacket with battery inserted");
        }

        int jackPerc =  ( jackPercent * ( width() - 47 ) ) / 100;

        qDrawShadePanel( &p,   9, 90, rightEnd1, 39, colorGroup(), TRUE, 1, NULL);
        qDrawShadePanel( &p, rightEnd2, 97,  12, 24, colorGroup(), TRUE, 1, NULL);
        drawSegment( &p, QRect( 10, 90, jackPerc, 40 ), lightc, darkc, lightc.light(115), 6 );
        drawSegment( &p, QRect( 11 + jackPerc, 90, rightEnd1 - jackPerc, 40 ), white.light(80), black, white.light(90), 6 );
        drawSegment( &p, QRect( rightEnd2, 97, 10, 25 ), white.light(80), black, white.light(90), 2 );
        p.setPen( black );
        p.drawText(15, 100, width() - 20, 20 , AlignVCenter,  jacketMsg);
    }
}

QSize BatteryStatus::sizeHint() const {
    QString text = statusText();
    QString text2 = statusTextIpaq();
    QFontMetrics fm = fontMetrics();
    QRect r=fm.boundingRect( 10, 0, width(), height(), AlignVCenter, text );
    QRect r2=fm.boundingRect( 10, 0, width(), height(), AlignVCenter, text2 );

    if ( bat2 )  {
    return QSize( QMAX( QMIN( 200, qApp->desktop()->width() ),
                        r.width() ), 2 * 10 + 80 + r.height() +  r2.height() );
    }
    return QSize( QMAX( QMIN( 200, qApp->desktop()->width() ),
                        r.width() ), 2 * 10 + 40 + r.height() );
}
