/*
 * addresspluginwidget.cpp
 *
 * copyright   : (c) 2003 by Stefan Eilers
 * email       : eilers.stefan@epost.de
 *
 * This implementation was derived from the todolist plugin implementation
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

#include "addresspluginwidget.h"

#include <qvaluelist.h>
#include <qtl.h>
#include <qstring.h>
#include <qscrollview.h>
#include <qobject.h>
#include <qdatetime.h> 

#include <qpe/config.h>
#include <qpe/timestring.h>
#include <qpe/qcopenvelope_qws.h>

#include <opie/ocontact.h>

AddressBookPluginWidget::AddressBookPluginWidget( QWidget *parent,  const char* name )
    : QWidget( parent, name ) {

    addressLabel = 0l;
    m_contactdb = 0l;
    layoutTodo = 0l;

    // Hä ? Nonsense ! (se)
    if ( m_contactdb ) {
        delete m_contactdb;
    }

    m_contactdb = new OContactAccess("addressplugin");
    
    connect( m_contactdb, SIGNAL( signalChanged( const OContactAccess * ) ),
	     this, SLOT( refresh( const OContactAccess * ) ) );


    readConfig();
    getAddress();
}

AddressBookPluginWidget::~AddressBookPluginWidget() {
    delete m_contactdb;
}

void AddressBookPluginWidget::refresh( const OContactAccess* )
{
	qWarning(" AddressBookPluginWidget::Database was changed externally ! ");
	m_contactdb->reload();
	getAddress();
}


void AddressBookPluginWidget::readConfig() {
    Config cfg( "todayaddressplugin" );
    cfg.setGroup( "config" );
    m_maxLinesTask =  cfg.readNumEntry( "maxlinestask", 5 );
    m_maxCharClip =   cfg.readNumEntry( "maxcharclip", 38 );
    m_daysLookAhead = cfg.readNumEntry( "dayslookahead", 14 );
}


/**
 * Get the addresss
 */
void AddressBookPluginWidget::getAddress() {

	if ( ! layoutTodo ){
		layoutTodo = new QVBoxLayout( this );
	}

	if ( ! addressLabel ) {
		addressLabel = new OClickableLabel( this );
		connect( addressLabel, SIGNAL( clicked() ), this, SLOT( startAddressBook() ) );
		layoutTodo->addWidget( addressLabel );
	}
	
	QString output;

	// Check whether the database provide the search option..
	if ( ! m_contactdb->hasQuerySettings( OContactAccess::DateDiff ) ){

		// Define the query for birthdays and start search..
		QDate lookAheadDate = QDate::currentDate().addDays( m_daysLookAhead );
		qWarning("Searching from now (%s) until %s ! ", QDate::currentDate().toString().latin1(), 
			 lookAheadDate.toString().latin1() );
		OContact querybirthdays;
		querybirthdays.setBirthday( lookAheadDate );

		int ammount = 0;
		
		m_list = m_contactdb->queryByExample( querybirthdays, OContactAccess::DateDiff );
		if ( m_list.count() > 0 ){
			output = QObject::tr( "Next birthdays in <b> %1 </b> days: <br>" ).arg( m_daysLookAhead );
			for ( m_it = m_list.begin(); m_it != m_list.end(); ++m_it ) {
				if ( ammount++ < m_maxLinesTask ){
					// Now we want to calculate how many days until birthday. We have to set 
					// the correct year to calculate the day diff...
					QDate destdate = (*m_it).birthday();
					destdate.setYMD( QDate::currentDate().year(), destdate.month(), destdate.day() );
					if ( QDate::currentDate().daysTo(destdate) < 0 )
						destdate.setYMD( QDate::currentDate().year()+1, destdate.month(), destdate.day() );

					output += "<font color=#e00000><b>-" + (*m_it).fullName() 
						+ " (" 
						+ QString::number(QDate::currentDate().daysTo(destdate)) 
						+ " Days) </b></font><br>";
				}
			}
		} else {
			output = QObject::tr( "No birthdays in <b> %1 </b> days! <br>" ).arg( m_daysLookAhead );
		}
		
		// Define the query for anniversaries and start search..
		OContact queryanniversaries;
		queryanniversaries.setAnniversary( lookAheadDate );
		
		m_list = m_contactdb->queryByExample( queryanniversaries, OContactAccess::DateDiff );
		
		ammount = 0;
		if ( m_list.count() > 0 ){
			output += QObject::tr( "Next anniversaries in <b> %1 </b> days: <br>" ).arg( m_daysLookAhead );
			for ( m_it = m_list.begin(); m_it != m_list.end(); ++m_it ) {
				if ( ammount++ < m_maxLinesTask ){
					// Now we want to calculate how many days until anniversary. We have to set 
					// the correct year to calculate the day diff...
					QDate destdate = (*m_it).anniversary();
					destdate.setYMD( QDate::currentDate().year(), destdate.month(), destdate.day() );
					if ( QDate::currentDate().daysTo(destdate) < 0 )
						destdate.setYMD( QDate::currentDate().year()+1, destdate.month(), destdate.day() );

					output += "<font color=#e00000><b>-" + (*m_it).fullName() 
						+ " (" 
						+ QString::number(QDate::currentDate().daysTo( destdate ) ) 
						+ " Days) </b></font><br>";
				}
			}
		} else {
			output += QObject::tr( "No anniversaries in <b> %1 </b> days! <br>" ).arg( m_daysLookAhead );
		}


	}else{
		// Libopie seems to be old..
		output = QObject::tr( "Database does not provide this search query ! Please upgrade libOpie !<br>" );
	}
	
	addressLabel->setText( output );
}

/**
 * start the todolist
 */
void AddressBookPluginWidget::startAddressBook() {
    QCopEnvelope e( "QPE/System", "execute(QString)" );
    e << QString( "addressbook" );
}
