/*
 * datebookpluginwidget.cpp
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


#include "datebookpluginwidget.h"
#include "datebookevent.h"

#include <qpe/timestring.h>
#include <qpe/config.h>

#include <qdatetime.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtl.h>
#include <qscrollview.h>
#include <qtimer.h>

DatebookPluginWidget::DatebookPluginWidget( QWidget *parent,  const char* name)
    : QWidget(parent,  name ) {
    db = 0l;
    readConfig();
    getDates();
}

DatebookPluginWidget::~DatebookPluginWidget() {
    delete db;
}


void DatebookPluginWidget::readConfig() {
    Config cfg( "todaydatebookplugin" );
    cfg.setGroup( "config" );
    m_max_lines_meet = cfg.readNumEntry( "maxlinesmeet", 5 );
    m_show_location = cfg.readNumEntry( "showlocation", 1 );
    m_show_notes = cfg.readNumEntry( "shownotes", 0 );
    m_onlyLater = cfg.readNumEntry( "onlylater", 1 );
    m_moreDays = cfg.readNumEntry( "moredays", 0 );
}


/**
 *  Get all events that are in the datebook xml file for today
 */
void DatebookPluginWidget::getDates() {


    QDate date = QDate::currentDate();

     QVBoxLayout* layoutDates = new QVBoxLayout( this );
     layoutDates->setSpacing( 1 );
     layoutDates->setMargin( 1 );

    if ( db ) {
        delete db;
    }

    db = new DateBookDB;

    QValueList<EffectiveEvent> list = db->getEffectiveEvents( date, date.addDays( m_moreDays )  );

    qBubbleSort( list );

    Config config( "qpe" );

    int count=0;

    if ( list.count() > 0 ) {

        for ( QValueList<EffectiveEvent>::ConstIterator it=list.begin(); it!=list.end(); ++it ) {

            if ( count <= m_max_lines_meet ) {
                QTime time = QTime::currentTime();

                if ( !m_onlyLater ) {
                    count++;
                    DateBookEvent *l = new DateBookEvent( *it, this, m_show_location, m_show_notes );
                    layoutDates->addWidget( l );
                    QObject::connect ( l, SIGNAL( editEvent( const Event &) ), l, SLOT( editEventSlot( const Event &) ) );
                } else if ( ( time.toString() <= TimeString::dateString( (*it).event().end() ) ) ) {
                    count++;
                    // show only later appointments
                    DateBookEvent *l = new DateBookEvent( *it, this, m_show_location, m_show_notes );
                    layoutDates->addWidget( l );
                    QObject::connect ( l, SIGNAL( editEvent( const Event &) ), l, SLOT( editEventSlot( const Event &) ) );
                }
            }
        }
        if ( m_onlyLater && count == 0 ) {
            QLabel* noMoreEvents = new QLabel( this );
            noMoreEvents->setText( QObject::tr( "No more appointments today" ) );
            layoutDates->addWidget( noMoreEvents );
        }
    } else {
        QLabel* noEvents = new QLabel( this );
        noEvents->setText( QObject::tr( "No appointments today" ) );
        layoutDates->addWidget( noEvents );
    }
}

