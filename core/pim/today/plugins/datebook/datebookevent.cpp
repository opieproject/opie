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

#include <opie/odevice.h>

using namespace Opie;

DateBookEvent::DateBookEvent(const EffectiveEvent &ev,
			     QWidget* parent,
			     bool show_location,
			     bool show_notes,
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
    if ( show_location ) {
        msg += "<BR><i>" + (ev).location() + "</i>";
    }

    if ( ( TimeString::timeString( QTime( (ev).event().start().time() ) ) == "00:00" )
         &&  ( TimeString::timeString( QTime( (ev).event().end().time() ) ) == "23:59" ) ) {
        msg += "<br>All day";
    }  else {
        // start time of event
        QDate tempDate = (ev).event().start().date();
        msg += "<br>"
               + ampmTime( QTime( (ev).event().start().time() ) )
               // end time of event
               + "<b> - </b>" + ampmTime( QTime( (ev).event().end().time() ) )
               + differDate( tempDate );
    }

    // include possible note or not
    if ( show_notes ) {
        msg += "<br> <i>note</i>:" +( (ev).notes() ).mid( 0, maxCharClip );
    }
    setText( msg );
    connect( this, SIGNAL( clicked() ), this, SLOT( editMe() ) );
}


/**
 * AM/PM timestring conversion.
 * @param tm the timestring
 *†@return formatted to am/pm is system is set to it
 */
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

QString DateBookEvent::differDate( QDate date ) {
    QDate currentDate = QDate::currentDate();
    QString returnText = "<font color = #407DD9><b> ";
    int differDate = currentDate.daysTo( date );
    if ( currentDate.dayOfWeek() == date.dayOfWeek() ) {
        returnText += "" ;
        // not working right for recurring events
        //} else if ( differDate == 1 ) {
        //returnText += tr( "tomorrow" );
    } else {
        //returnText += tr( "in %1 days" ).arg( differDate );
        returnText += "   [ " + date.dayName( date.dayOfWeek() ) + " ] ";
    }
    returnText += "</b></font>";
    return returnText;
}


/**
 * starts the edit dialog as known from datebook
 */
void DateBookEvent::editEventSlot( const Event &e ) {

    if ( ODevice::inst()->system() == System_Zaurus ) {
        QCopEnvelope env( "QPE/Application/datebook", "raise()" );
    } else {
        QCopEnvelope env( "QPE/Datebook", "editEvent(int)" );
        env << e.uid();
    }
}


void DateBookEvent::editMe() {
    emit editEvent( event.event() );
}

