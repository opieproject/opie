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

#include <opie2/odebug.h>
#include <opie2/opimcontact.h>

#include <qpe/config.h>

AddressBookPluginWidget::AddressBookPluginWidget( QWidget *parent,  const char* name )
    : QWidget( parent, name ) {

    addressLabel = 0l;
    m_contactdb = 0l;
    layoutTodo = 0l;

    // Hä ? Nonsense ! (se)
    if ( m_contactdb ) {
        delete m_contactdb;
    }

    m_contactdb = new Opie::OPimContactAccess("addressplugin");

    connect( m_contactdb, SIGNAL( signalChanged(const Opie::OPimContactAccess*) ),
	     this, SLOT( refresh(const Opie::OPimContactAccess*) ) );


    readConfig();
    getAddress();
}

AddressBookPluginWidget::~AddressBookPluginWidget() {
    delete m_contactdb;
}

void AddressBookPluginWidget::refresh( const Opie::OPimContactAccess* )
{
	oerr << " AddressBookPluginWidget::Database was changed externally ! " << oendl;
	m_contactdb->reload();
	getAddress();
}

void AddressBookPluginWidget::reinitialize() {
	readConfig();
	getAddress();
}

void AddressBookPluginWidget::readConfig() {
    Config cfg( "todayaddressplugin" );
    cfg.setGroup( "config" );
    m_maxLinesTask  = cfg.readNumEntry( "maxlinestask", 5 );
    m_maxCharClip   = cfg.readNumEntry( "maxcharclip", 38 );
    m_daysLookAhead = cfg.readNumEntry( "dayslookahead", 14 );
    m_urgentDays    = cfg.readNumEntry( "urgentdays", 7 );
    m_entryColor    = cfg.readEntry("entrycolor", Qt::black.name() );
    m_headlineColor = cfg.readEntry( "headlinecolor", Qt::black.name() );
    m_urgentColor   = cfg.readEntry( "urgentcolor", Qt::red.name() );
    m_showBirthdays = cfg.readBoolEntry( "showBirthdays", true );
    m_showAnniversaries = cfg.readBoolEntry( "showAnniversaries", true );
}


/**
 * Get the addresss
 */
void AddressBookPluginWidget::getAddress() {

	if ( ! layoutTodo ){
		layoutTodo = new QVBoxLayout( this );
	}

	if ( ! addressLabel ) {
		addressLabel = new Opie::Ui::OClickableLabel( this );
		connect( addressLabel, SIGNAL( clicked() ), this, SLOT( startAddressBook() ) );
		layoutTodo->addWidget( addressLabel );
	}

	QString output;

	// Check whether the database provide the search option..
    if ( !m_contactdb->hasQuerySettings( Opie::OPimContactAccess::DateDiff ) ){
		// Libopie seems to be old..
		output = QObject::tr( "Database does not provide this search query ! Please upgrade libOpie !<br>" );
		addressLabel->setText( output );
		return;
	}

	// Define the query for birthdays and start search..
	QDate lookAheadDate = QDate::currentDate().addDays( m_daysLookAhead );
	int ammount = 0;
	if ( m_showBirthdays ){
		oerr << "Searching from now (" << QDate::currentDate().toString() << ") until "
						<< lookAheadDate.toString() << " ! " << oendl;
    }

    if ( m_contactdb->hasQuerySettings( Opie::OPimContactAccess::DateDiff ) ){


		Opie::OPimContact querybirthdays;
		querybirthdays.setBirthday( lookAheadDate );

		m_list = m_contactdb->queryByExample( querybirthdays,
						      Opie::OPimContactAccess::DateDiff );
		if ( m_list.count() > 0 ){
			output = "<font color=" + m_headlineColor + ">"
				+ QObject::tr( "Next birthdays in <b> %1 </b> days:" )
				.arg( m_daysLookAhead )
				+ "</font> <br>";
			for ( m_it = m_list.begin(); m_it != m_list.end(); ++m_it ) {
				if ( ammount++ < m_maxLinesTask ){
					// Now we want to calculate how many days
					//until birthday. We have to set
					// the correct year to calculate the day diff...
					QDate destdate = (*m_it).birthday();
					destdate.setYMD( QDate::currentDate().year(),
							 destdate.month(), destdate.day() );
					if ( QDate::currentDate().daysTo(destdate) < 0 )
						destdate.setYMD( QDate::currentDate().year()+1,
								 destdate.month(), destdate.day() );


					if ( QDate::currentDate().daysTo(destdate) < m_urgentDays )
						output += "<font color=" + m_urgentColor + "><b>-"
							+ (*m_it).fullName()
							+ " ("
							+ QString::number(QDate::currentDate()
									  .daysTo(destdate))
							+ " Days) </b></font><br>";

					else
						output += "<font color=" + m_entryColor + "><b>-"
							+ (*m_it).fullName()
							+ " ("
							+ QString::number(QDate::currentDate()
									  .daysTo(destdate))
							+ " Days) </b></font><br>";
				}
			}
		} else {
			output = "<font color=" + m_headlineColor + ">"
				+ QObject::tr( "No birthdays in <b> %1 </b> days!" )
				.arg( m_daysLookAhead )
				+ "</font> <br>";
		}
	}

	if ( m_showAnniversaries ){
		// Define the query for anniversaries and start search..
		Opie::OPimContact queryanniversaries;
		queryanniversaries.setAnniversary( lookAheadDate );

		m_list = m_contactdb->queryByExample( queryanniversaries, Opie::OPimContactAccess::DateDiff );

		ammount = 0;
		if ( m_list.count() > 0 ){
			output += "<font color=" + m_headlineColor + ">"
				+ QObject::tr( "Next anniversaries in <b> %1 </b> days:" )
				.arg( m_daysLookAhead )
				+ "</font> <br>";
			for ( m_it = m_list.begin(); m_it != m_list.end(); ++m_it ) {
				if ( ammount++ < m_maxLinesTask ){
					// Now we want to calculate how many days until anniversary.
					// We have to set the correct year to calculate the day diff...
					QDate destdate = (*m_it).anniversary();
					destdate.setYMD( QDate::currentDate().year(), destdate.month(),
							 destdate.day() );
					if ( QDate::currentDate().daysTo(destdate) < 0 )
						destdate.setYMD( QDate::currentDate().year()+1,
								 destdate.month(), destdate.day() );

					if ( QDate::currentDate().daysTo(destdate) < m_urgentDays )
						output += "<font color=" + m_urgentColor + "><b>-"
							+ (*m_it).fullName()
							+ " ("
							+ QString::number(QDate::currentDate()
									  .daysTo( destdate ) )
							+ " Days) </b></font><br>";
					else
						output += "<font color=" + m_entryColor + "><b>-"
							+ (*m_it).fullName()
							+ " ("
							+ QString::number(QDate::currentDate()
									  .daysTo( destdate ) )
							+ " Days) </b></font><br>";
				}
			}
		} else {
			output += "<font color=" + m_headlineColor + ">"
				+ QObject::tr( "No anniversaries in <b> %1 </b> days!" )
				.arg( m_daysLookAhead )
				+ "</font> <br>";
		}
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
