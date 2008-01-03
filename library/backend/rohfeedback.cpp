/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include <rohfeedback.h>
#include <stdio.h>
#include <qpeapplication.h>
#include <qevent.h>
#include <resource.h>
#include <qpixmap.h>
#include <qbitmap.h>

#define SPEED 500
#define DELAY 200

namespace Opie {
namespace Internal {
/*

    RightOnHold feedback

*/

QPixmap * RoHFeedback::Imgs[NOOFICONS] = { 0, 0, 0, 0, 0 };
QBitmap * RoHFeedback::Masks[NOOFICONS];
int RoHFeedback::IconWidth;
int RoHFeedback::IconHeight;

RoHFeedback::RoHFeedback() :
                        QLabel( 0, 0, Qt::WType_Popup ), Timer() {

    Receiver = 0l;
    connect( &Timer, SIGNAL( timeout() ), this, SLOT( iconShow() ) );

    if( Imgs[0] == 0 ) {
      QString S;


      for( int i = 0; i < NOOFICONS ; i ++ ) {
        Imgs[i] =  new QPixmap( Resource::loadPixmap(
              "RoH/star/"+ QString::number(i+1)  ));
        Masks[i] = new QBitmap();
        (*Masks[i]) = Resource::loadPixmap(
              "RoH/star/"+QString::number(i+1) + "-mask" );
      }
    }

    IconWidth = Imgs[0]->size().width();
    IconHeight = Imgs[0]->size().height();

    resize( IconWidth, IconHeight );
}

int RoHFeedback::delay( void ) {
    return DELAY+SPEED+50;
}

RoHFeedback::~RoHFeedback() {
    for ( int i = 0; i < NOOFICONS; ++i ) {
        delete Imgs [i];
        delete Masks[i];
    }
}

void RoHFeedback::init( const QPoint & P, QWidget* wid ) {
    if( ! IconWidth )
      return;

    Receiver =  wid;
    IconNr = FeedbackTimerStart;
    move( P.x()-IconWidth/2, P.y() - IconHeight/2 );
    // to initialize
    Timer.start( DELAY );
}

void RoHFeedback::stop( void ) {
    IconNr = FeedbackStopped; // stop
}

bool RoHFeedback::event( QEvent * E ) {

    if( E->type() >= QEvent::MouseButtonPress &&
        E->type() <= QEvent::MouseMove ) {
      // pass the event to the receiver with translated coord
      QMouseEvent QME( ((QMouseEvent *)E)->type(),
                       Receiver->mapFromGlobal(
                          ((QMouseEvent *)E)->globalPos() ),
                       ((QMouseEvent *)E)->globalPos(),
                       ((QMouseEvent *)E)->button(),
                       ((QMouseEvent *)E)->state()
                   );
      return QPEApplication::sendEvent( Receiver, &QME );
    }

    // first let the label treat the event
    return QLabel::event( E );
}

void RoHFeedback::iconShow( void ) {
    switch( IconNr ) {
      case FeedbackTimerStart:
        IconNr = FeedbackShow;
        Timer.start( SPEED/NOOFICONS );
        break;
      case FeedbackStopped:
        // stopped
        IconNr = FeedbackTimerStart;
        hide();
        Timer.stop();
        break;
      case FeedbackShow: // first
        show();
        // FT
      default :
        // show

        setPixmap( *(Imgs[IconNr]) );
        setMask( *(Masks[IconNr]) );
        IconNr = (IconNr+1)%NOOFICONS; // rotate
        break;
    }
}

}
}

