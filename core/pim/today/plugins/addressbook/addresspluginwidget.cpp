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
    : QWidget( parent, name )
{
    addressLabel = 0l;
    m_contactdb = 0l;
    layoutTodo = 0l;

    m_contactdb = new Opie::OPimContactAccess("addressplugin");
    m_contactdb->load();

    connect( m_contactdb, SIGNAL( signalChanged(const Opie::OPimContactAccess*) ),
             this, SLOT( refresh(const Opie::OPimContactAccess*) ) );

    readConfig();
    getAddress();
}

AddressBookPluginWidget::~AddressBookPluginWidget()
{
    delete m_contactdb;
}

void AddressBookPluginWidget::refresh( const Opie::OPimContactAccess* )
{
    owarn << " AddressBookPluginWidget::Database was changed externally ! " << oendl;
    m_contactdb->reload();
    getAddress();
}

void AddressBookPluginWidget::reinitialize()
{
    readConfig();
    getAddress();
}

void AddressBookPluginWidget::readConfig()
{
    Config cfg( "todayaddressplugin" );
    cfg.setGroup( "config" );
    m_maxLinesTask  = cfg.readNumEntry( "maxlinestask", 5 );
    m_maxCharClip   = cfg.readNumEntry( "maxcharclip", 38 );
    m_daysLookAhead = cfg.readNumEntry( "dayslookahead", 14 );
    m_urgentDays    = cfg.readNumEntry( "urgentdays", 7 );
    m_entryColor    = cfg.readEntry( "entrycolor", Qt::black.name() );
    m_headlineColor = cfg.readEntry( "headlinecolor", Qt::black.name() );
    m_urgentColor   = cfg.readEntry( "urgentcolor", Qt::red.name() );
    m_showBirthdays = cfg.readBoolEntry( "showBirthdays", true );
    m_showAnniversaries = cfg.readBoolEntry( "showAnniversaries", true );
}


/**
 * Get the addresss
 */
void AddressBookPluginWidget::getAddress()
{
    if ( ! layoutTodo ) {
        layoutTodo = new QVBoxLayout( this );
    }

    if ( ! addressLabel ) {
        addressLabel = new Opie::Ui::OClickableLabel( this );
        connect( addressLabel, SIGNAL( clicked() ), this, SLOT( startAddressBook() ) );
        layoutTodo->addWidget( addressLabel );
    }

    QString output;

    // Check whether the database provide the search option..
    if ( !m_contactdb->hasQuerySettings( Opie::OPimContactAccess::DateDiff ) ) {
        // Libopie seems to be old..
        output = QObject::tr( "Database does not provide this search query ! Please upgrade libOpie !<br>" );
        addressLabel->setText( output );
        return;
    }

    // Get owner contact, in case that has an anniversary/birthday defined
    QDate ownerBirthday;
    QDate ownerAnniversary;
    Opie::OPimContact ownerContact = Opie::OPimContactAccess::businessCard();
    if( !ownerContact.isEmpty() ) {
        ownerBirthday = ownerContact.birthday();
        if( daysToNext( ownerBirthday ) > m_daysLookAhead )
            ownerBirthday = QDate();
        ownerAnniversary = ownerContact.anniversary();
        if( daysToNext( ownerAnniversary ) > m_daysLookAhead )
            ownerAnniversary = QDate();
    }

    // Define the query for birthdays and start search..
    QDate lookAheadDate = QDate::currentDate().addDays( m_daysLookAhead );
    int lines = 0;
    Opie::OPimContactAccess::List list;
    Opie::OPimContactAccess::List::Iterator it;
    QValueList<Opie::OPimContact>::Iterator it2;

    if ( m_showBirthdays ) {
        owarn << "Searching from now (" << QDate::currentDate().toString() << ") until "
                << lookAheadDate.toString() << " ! " << oendl;

        if ( m_contactdb->hasQuerySettings( Opie::OPimContactAccess::DateDiff ) ) {
            Opie::OPimContact querybirthdays;
            querybirthdays.setBirthday( lookAheadDate );

            list = m_contactdb->queryByExample( querybirthdays,
                                                  Opie::OPimContactAccess::DateDiff );

            if ( list.count() > 0 || !ownerBirthday.isNull() ) {
                output = "<font color=" + m_headlineColor + ">"
                    + QObject::tr( "Birthdays in next %1 days:" )
                    .arg( m_daysLookAhead )
                    + "</font> <br>";

                // Sort filtered results
                list = m_contactdb->sorted( list, true, Opie::OPimContactAccess::SortBirthdayWithoutYear,
                                              Opie::OPimContactAccess::FilterOff, 0 );

                // Construct a new list containing the owner's birthday if any
                QValueList<Opie::OPimContact> list2;
                for ( it = list.begin(); it != list.end(); ++it ) {
                    if( (! ownerBirthday.isNull()) && (*it).birthday() > ownerBirthday ) {
                        list2.append( ownerContact );
                        ownerBirthday = QDate();
                    }
                    list2.append(*it);
                }
                if( ! ownerBirthday.isNull() )
                    list2.append( ownerContact );

                // List the birthdays
                for ( it2 = list2.begin(); it2 != list2.end(); ++it2 ) {
                    if ( lines++ < m_maxLinesTask ) {
                        output += itemText( (*it2).birthday(), (*it2).fullName() );
                    }
                }
            }
            else {
                output = "<font color=" + m_headlineColor + ">"
                    + QObject::tr( "No birthdays in next %1 days" )
                    .arg( m_daysLookAhead )
                    + "</font> <br>";
            }
        }
    }

    if ( m_showAnniversaries ) {
        // Define the query for anniversaries and start search..
        Opie::OPimContact queryanniversaries;
        queryanniversaries.setAnniversary( lookAheadDate );

        list = m_contactdb->queryByExample( queryanniversaries, Opie::OPimContactAccess::DateDiff );

        lines = 0;
        if ( list.count() > 0 || !ownerAnniversary.isNull()  ) {
            output += "<font color=" + m_headlineColor + ">"
                + QObject::tr( "Anniversaries in next %1 days:" )
                .arg( m_daysLookAhead )
                + "</font> <br>";

            // Sort filtered results
            list = m_contactdb->sorted( list, true, Opie::OPimContactAccess::SortAnniversaryWithoutYear,
                              Opie::OPimContactAccess::FilterOff, 0 );

            // Construct a new list containing the owner's anniversary if any
            QValueList<Opie::OPimContact> list2;
            for ( it = list.begin(); it != list.end(); ++it ) {
                if( (! ownerAnniversary.isNull()) && (*it).anniversary() > ownerAnniversary ) {
                    list2.append( ownerContact );
                    ownerAnniversary = QDate();
                }
                list2.append(*it);
            }
            if( ! ownerAnniversary.isNull() )
                list2.append( ownerContact );

            // List the anniversaries
            for ( it2 = list2.begin(); it2 != list2.end(); ++it2 ) {
                if ( lines++ < m_maxLinesTask ) {
                    output += itemText( (*it2).anniversary(), (*it2).fullName() );
                }
            }
        }
        else {
            output += "<font color=" + m_headlineColor + ">"
                + QObject::tr( "No anniversaries in next %1 days" )
                .arg( m_daysLookAhead )
                + "</font> <br>";
        }
    }

    addressLabel->setText( output );
}

int AddressBookPluginWidget::daysToNext( const QDate &date )
{
    // Calculate how many days until the birthday/anniversary
    // We have to set the correct year to calculate the day diff...
    QDate destdate;
    destdate.setYMD( QDate::currentDate().year(), date.month(),
                date.day() );
    if ( QDate::currentDate().daysTo(destdate) < 0 )
        destdate.setYMD( QDate::currentDate().year()+1,
                    destdate.month(), destdate.day() );

    return QDate::currentDate().daysTo(destdate);
}

QString AddressBookPluginWidget::itemText( const QDate &date, const QString &desc )
{
    int daysleft = daysToNext( date );

    QString color = m_entryColor;
    if ( daysleft < m_urgentDays )
        color = m_urgentColor;

    QString output = "<font color=" + color + "><b>- " + desc + " (";

    if( daysleft == 0 )
        output += QObject::tr("today");
    else if( daysleft == 1 )
        output += QObject::tr("tomorrow");
    else
        output += QObject::tr("%1 days").arg(daysleft);

    output += ") </b></font><br>";

    return output;
}

/**
 * start the todolist
 */
void AddressBookPluginWidget::startAddressBook()
{
    QCopEnvelope e( "QPE/System", "execute(QString)" );
    e << QString( "addressbook" );
}
