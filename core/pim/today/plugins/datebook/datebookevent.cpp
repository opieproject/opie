/*
 * datebookevent.cpp
 *
 * copyright   : (c) 2002 by Maximilian Reiﬂ
 * email       : harlekin@handhelds.org
 *
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "datebookevent.h"
#include <qpe/config.h>
#include <qpe/timestring.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>

DateBookEvent::DateBookEvent(const EffectiveEvent &ev,
			     QWidget* parent,
			     int show_location,
			     int show_notes,
                             //  int onlyLater,
                             int maxCharClip,
                             const char* name,
			     WFlags fl) :
    OClickableLabel(parent,name,fl), event(ev) {

    setAlignment( AlignTop );

    QString msg;

    Config config( "qpe" );
    config.setGroup( "Time" );
    // if 24 h format
    ampm = config.readBoolEntry( "AMPM", TRUE );

    msg += "<B>" + (ev).description() + "</B>";
    if ( (ev).event().hasAlarm() ) {
        msg += " <b>[with alarm]</b>";
    }

    // include location or not
    if ( show_location == 1) {
        msg += "<BR><i>" + (ev).location() + "</i>";
    }

    if ( ( TimeString::timeString( QTime( (ev).event().start().time() ) ) == "00:00" )
         &&  ( TimeString::timeString( QTime( (ev).event().end().time() ) ) == "23:59") ) {
        msg += "<br>All day";
    }  else {
        // start time of event
        msg += "<br>" + ampmTime(QTime( (ev).event().start().time() ) )
               // end time of event
               + "<b> - </b>" + ampmTime(QTime( (ev).event().end().time() ) );
    }

    // include possible note or not
    if ( show_notes == 1) {
        msg += "<br> <i>note</i>:" +( (ev).notes() ).mid( 0, maxCharClip );
    }

    setText( msg );
    connect( this, SIGNAL( clicked() ), this, SLOT( editMe() ) );
    setAlignment( int( QLabel::WordBreak | QLabel::AlignLeft ) );
}


QString DateBookEvent::ampmTime( QTime tm ) {

    QString s;
    if( ampm ) {
        int hour = tm.hour();
        if ( hour == 0 ) {
            hour = 12;
        }
        if ( hour > 12 ) {
            hour -= 12;
        }
        s.sprintf( "%2d:%02d %s", hour, tm.minute(),
                   (tm.hour() >= 12) ? "PM" : "AM" );
        return s;
    } else {
        s.sprintf( "%2d:%02d", tm.hour(), tm.minute() );
        return s;
    }

}


/*
 * starts the edit dialog as known from datebook
 */
void DateBookEvent::editEventSlot( const Event &e ) {
  startDatebook();

  while( !QCopChannel::isRegistered( "QPE/Datebook" ) ) qApp->processEvents();
  QCopEnvelope env( "QPE/Datebook", "editEvent(int)" );
  env << e.uid();
}


/**
 * launches datebook
 */
void DateBookEvent::startDatebook() {
    QCopEnvelope e("QPE/System", "execute(QString)");
    e << QString("datebook");
}

void DateBookEvent::editMe() {
  emit editEvent(event.event());
}

