 /*
 * datebookpluginwidget.cpp
 *
 * copyright   : (c) 2002,2003 by Maximilian Reiﬂ
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

#include <qpe/timestring.h>
#include <qpe/config.h>

#include <qdatetime.h>
#include <qlabel.h>
#include <qtl.h>

DatebookPluginWidget::DatebookPluginWidget( QWidget *parent, const char* name )
    : QWidget(parent, name ) {

    db = 0l;
    m_layoutDates = 0l;

    if ( db ) {
        delete db;
    }
    db = new DateBookDB;

    if ( m_layoutDates )  {
        delete m_layoutDates;
    }
    m_layoutDates = new QVBoxLayout( this );
    m_layoutDates->setAutoAdd( true );

    m_eventsList.setAutoDelete( true );

    readConfig();
    getDates();
}

DatebookPluginWidget::~DatebookPluginWidget() {
    delete db;
    delete m_layoutDates;
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

void DatebookPluginWidget::reinitialize()  {
    readConfig();
    refresh();
}

void DatebookPluginWidget::refresh()  {
    m_eventsList.clear();

    if ( m_layoutDates )  {
        delete m_layoutDates;
    }
    m_layoutDates = new QVBoxLayout( this );
    m_layoutDates->setAutoAdd( true );

    getDates();
}

/**
 *  Get all events that are in the datebook xml file for today
 */
void DatebookPluginWidget::getDates() {

    QDate date = QDate::currentDate();
    QValueList<EffectiveEvent> list = db->getEffectiveEvents( date, date.addDays( m_moreDays )  );
    qBubbleSort( list );
    int count = 0;

    if ( list.count() > 0 ) {

        for ( QValueList<EffectiveEvent>::ConstIterator it = list.begin(); it  != list.end(); ++it ) {

            if ( count < m_max_lines_meet ) {
                if ( !m_onlyLater ) {
                    count++;
                    DateBookEvent *l = new DateBookEvent( *it, this, m_show_location, m_show_notes );
                    m_eventsList.append( l );
                    l->show();
                    QObject::connect ( l, SIGNAL( editEvent( const Event & ) ), l, SLOT( editEventSlot( const Event & ) ) );
                } else if ( ( QDateTime::currentDateTime()  <=  (*it).event().end() )  || ( ( (*it).event().start().date() != date ) &&  (  QDateTime::currentDateTime()  <=  (*it).event().end() )  )  ) {
                    count++;
                    // show only later appointments
                    DateBookEvent *l = new DateBookEvent( *it, this, m_show_location, m_show_notes );
                    m_eventsList.append( l );
                    l->show();
                    QObject::connect ( l, SIGNAL( editEvent( const Event & ) ), l, SLOT( editEventSlot( const Event & ) ) );
                }
            }
        }
        if ( m_onlyLater && count == 0 ) {
            QLabel* noMoreEvents = new QLabel( this );
            m_eventsList.append(  noMoreEvents );
            noMoreEvents->show();
            noMoreEvents->setText( QObject::tr( "No more appointments today" ) );
        }
    } else {
        QLabel* noEvents = new QLabel( this );
        m_eventsList.append( noEvents );
        noEvents->show();
        noEvents->setText( QObject::tr( "No appointments today" ) );
    }
}


