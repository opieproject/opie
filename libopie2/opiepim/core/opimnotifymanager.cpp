/*
                             This file is part of the Opie Project
                             Copyright (C) The Main Author <main-author@whereever.org>
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include "opimnotifymanager.h"

/* OPIE */
#include <opie2/oconversion.h>

/* QT */
#include <qstringlist.h>

namespace Opie
{

OPimNotifyManager::OPimNotifyManager( const Reminders& rem, const Alarms& al )
        : m_rem( rem ), m_al( al )
{}


OPimNotifyManager::~OPimNotifyManager()
{}


/* use static_cast and type instead of dynamic... */
void OPimNotifyManager::add( const OPimNotify& noti )
{
    if ( noti.type() == QString::fromLatin1( "OPimReminder" ) )
    {
        const OPimReminder & rem = static_cast<const OPimReminder&>( noti );
        m_rem.append( rem );
    }
    else if ( noti.type() == QString::fromLatin1( "OPimAlarm" ) )
    {
        const OPimAlarm & al = static_cast<const OPimAlarm&>( noti );
        m_al.append( al );
    }
}


void OPimNotifyManager::remove( const OPimNotify& noti )
{
    if ( noti.type() == QString::fromLatin1( "OPimReminder" ) )
    {
        const OPimReminder & rem = static_cast<const OPimReminder&>( noti );
        m_rem.remove( rem );
    }
    else if ( noti.type() == QString::fromLatin1( "OPimAlarm" ) )
    {
        const OPimAlarm & al = static_cast<const OPimAlarm&>( noti );
        m_al.remove( al );
    }
}


void OPimNotifyManager::replace( const OPimNotify& noti )
{
    if ( noti.type() == QString::fromLatin1( "OPimReminder" ) )
    {
        const OPimReminder & rem = static_cast<const OPimReminder&>( noti );
        m_rem.remove( rem );
        m_rem.append( rem );
    }
    else if ( noti.type() == QString::fromLatin1( "OPimAlarm" ) )
    {
        const OPimAlarm & al = static_cast<const OPimAlarm&>( noti );
        m_al.remove( al );
        m_al.append( al );
    }
}


OPimNotifyManager::Reminders OPimNotifyManager::reminders() const
{
    return m_rem;
}


OPimNotifyManager::Alarms OPimNotifyManager::alarms() const
{
    return m_al;
}


OPimAlarm OPimNotifyManager::alarmAtDateTime( const QDateTime& when, bool& found ) const
{
    Alarms::ConstIterator it;
    found = true;

    for ( it = m_al.begin(); it != m_al.end(); ++it )
    {
        if ( ( *it ).dateTime() == when )
            return ( *it );
    }

    // Fall through if nothing could be found
    found = false;
    OPimAlarm empty;
    return empty;
}


void OPimNotifyManager::setAlarms( const Alarms& al )
{
    m_al = al;
}


void OPimNotifyManager::setReminders( const Reminders& rem )
{
    m_rem = rem;
}


/* FIXME!!! */
/**
 * The idea is to check if the provider for our service
 * is online
 * if it is we will use QCOP
 * if not the Factory to get the backend...
 * Qtopia1.6 services would be kewl to have here....
 */
void OPimNotifyManager::registerNotify( const OPimNotify& )
{
}


/* FIXME!!! */
/**
 * same as above...
 * Also implement Url model
 * have a MainWindow....
 */
void OPimNotifyManager::deregister( const OPimNotify& )
{
}


bool OPimNotifyManager::isEmpty() const
{
    qWarning( "is Empty called on OPimNotifyManager %d %d", m_rem.count(), m_al.count() );
    if ( m_rem.isEmpty() && m_al.isEmpty() ) return true;
    else return false;
}


// Taken from otodoaccessxml.. code duplication bad. any alternative?
QString OPimNotifyManager::alarmsToString() const
{
    QString str;

    OPimNotifyManager::Alarms alarms = m_al;
    if ( !alarms.isEmpty() )
    {
        QStringList als;
        OPimNotifyManager::Alarms::Iterator it = alarms.begin();
        for ( ; it != alarms.end(); ++it )
        {
            /* only if time is valid */
            if ( ( *it ).dateTime().isValid() )
            {
                als << OConversion::dateTimeToString( ( *it ).dateTime() )
                + ":" + QString::number( ( *it ).duration() )
                + ":" + QString::number( ( *it ).sound() )
                + ":";
            }
        }
        // now write the list
        qWarning( "als: %s", als.join( "____________" ).latin1() );
        str = als.join( ";" );
    }

    return str;
}


QString OPimNotifyManager::remindersToString() const
{
    QString str;

    OPimNotifyManager::Reminders reminders = m_rem;
    if ( !reminders.isEmpty() )
    {
        OPimNotifyManager::Reminders::Iterator it = reminders.begin();
        QStringList records;
        for ( ; it != reminders.end(); ++it )
        {
            records << QString::number( ( *it ).recordUid() );
        }
        str = records.join( ";" );
    }

    return str;
}


void OPimNotifyManager::alarmsFromString( const QString& str )
{
    QStringList als = QStringList::split( ";", str );
    for ( QStringList::Iterator it = als.begin(); it != als.end(); ++it )
    {
        QStringList alarm = QStringList::split( ":", ( *it ), TRUE ); // allow empty
        qWarning( "alarm: %s", alarm.join( "___" ).latin1() );
        qWarning( "alarm[0]: %s %s", alarm[ 0 ].latin1(),
                  OConversion::dateTimeFromString( alarm[ 0 ] ).toString().latin1() );
        OPimAlarm al( alarm[ 2 ].toInt(), OConversion::dateTimeFromString( alarm[ 0 ] ),
                      alarm[ 1 ].toInt() );
        add( al );
    }
}


void OPimNotifyManager::remindersFromString( const QString& str )
{

    QStringList rems = QStringList::split( ";", str );
    for ( QStringList::Iterator it = rems.begin(); it != rems.end(); ++it )
    {
        OPimReminder rem( ( *it ).toInt() );
        add( rem );
    }

}
}
