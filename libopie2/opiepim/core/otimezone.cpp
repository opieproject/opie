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

#include "otimezone.h"

/* STD */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

namespace Opie
{

QDateTime utcTime( time_t t )
{
    tm * broken = ::gmtime( &t );
    QDateTime ret;
    ret.setDate( QDate( broken->tm_year + 1900, broken->tm_mon + 1, broken->tm_mday ) );
    ret.setTime( QTime( broken->tm_hour, broken->tm_min, broken->tm_sec ) );
    return ret;
}
QDateTime utcTime( time_t t, const QString& zone )
{
    QCString org = ::getenv( "TZ" );
#ifndef Q_OS_MACX   // Following line causes bus errors on Mac

    ::setenv( "TZ", zone.latin1(), true );
    ::tzset();

    tm* broken = ::localtime( &t );
    ::setenv( "TZ", org, true );
#else
#warning "Need a replacement for MacOSX!!"

    tm* broken = ::localtime( &t );
#endif

    QDateTime ret;
    ret.setDate( QDate( broken->tm_year + 1900, broken->tm_mon + 1, broken->tm_mday ) );
    ret.setTime( QTime( broken->tm_hour, broken->tm_min, broken->tm_sec ) );

    return ret;
}
time_t to_Time_t( const QDateTime& utc, const QString& str )
{
    QDate d = utc.date();
    QTime t = utc.time();

    tm broken;
    broken.tm_year = d.year() - 1900;
    broken.tm_mon = d.month() - 1;
    broken.tm_mday = d.day();
    broken.tm_hour = t.hour();
    broken.tm_min = t.minute();
    broken.tm_sec = t.second();

    QCString org = ::getenv( "TZ" );
#ifndef Q_OS_MACX   // Following line causes bus errors on Mac

    ::setenv( "TZ", str.latin1(), true );
    ::tzset();

    time_t ti = ::mktime( &broken );
    ::setenv( "TZ", org, true );
#else
#warning "Need a replacement for MacOSX!!"

    time_t ti = ::mktime( &broken );
#endif

    return ti;
}
}

namespace Opie
{
OTimeZone::OTimeZone( const ZoneName& zone )
        : m_name( zone )
{}


OTimeZone::~OTimeZone()
{}


bool OTimeZone::isValid() const
{
    return !m_name.isEmpty();
}

/*
 * we will get the current timezone
 * and ask it to convert to the timezone date
 */
QDateTime OTimeZone::toLocalDateTime( const QDateTime& dt )
{
    return OTimeZone::current().toDateTime( dt, *this );
}


QDateTime OTimeZone::toUTCDateTime( const QDateTime& dt )
{
    return OTimeZone::utc().toDateTime( dt, *this );
}


QDateTime OTimeZone::fromUTCDateTime( time_t t )
{
    return utcTime( t );
}


QDateTime OTimeZone::toDateTime( time_t t )
{
    return utcTime( t, m_name );
}


/*
 * convert dt to utc using zone.m_name
 * convert utc -> timeZoneDT using this->m_name
 */
QDateTime OTimeZone::toDateTime( const QDateTime& dt, const OTimeZone& zone )
{
    time_t utc = to_Time_t( dt, zone.m_name );
    qWarning( "%d %s", utc, zone.m_name.latin1() );
    return utcTime( utc, m_name );
}


time_t OTimeZone::fromDateTime( const QDateTime& time )
{
    return to_Time_t( time, m_name );
}


time_t OTimeZone::fromUTCDateTime( const QDateTime& time )
{
    return to_Time_t( time, "UTC" );
}


OTimeZone OTimeZone::current()
{
    QCString str = ::getenv( "TZ" );
    OTimeZone zone( str );
    return zone;
}


OTimeZone OTimeZone::utc()
{
    return OTimeZone( "UTC" );
}


QString OTimeZone::timeZone() const
{
    return m_name;
}

}
