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

#ifndef OTIMEZONE_H
#define OTIMEZONE_H

/* QT */
#include <qdatetime.h>

/* STD */
#include <time.h>

namespace Opie
{
/**
 * A very primitive class to convert time
 * from one timezone to another
 * and to localtime
 * and time_t
 */
class OTimeZone {

 public:
    typedef QString ZoneName;
    OTimeZone( const ZoneName&  = ZoneName::null );
    virtual ~OTimeZone(); // just in case.

    bool isValid()const;

    /**
     * converts the QDateTime to a DateTime
     * in the local timezone
     * if QDateTime is 25th Jan and takes place in Europe/Berlin at 12h
     * and the current timezone is Europe/London the returned
     * time will be 11h.
     */
    QDateTime toLocalDateTime( const QDateTime& dt );

    /**
     * converts the QDateTime to UTC time
     */
    QDateTime toUTCDateTime( const QDateTime& dt );

    /**
     * reads the time_t into a QDateTime using UTC as timezone!
     */
    QDateTime fromUTCDateTime( time_t );

    /**
     * converts the time_t to the time in the timezone
     */
    QDateTime toDateTime( time_t );

    /**
     * converts the QDateTime from one timezone to this timeZone
     */
    QDateTime toDateTime( const QDateTime&, const OTimeZone& timeZone );

    /**
     * converts the date time into a time_t. It takes the timezone into account
     */
    time_t fromDateTime( const QDateTime& );

    /**
     * converts the datetime with timezone UTC
     */
    time_t fromUTCDateTime( const QDateTime& );

    static OTimeZone current();
    static OTimeZone utc();

    QString timeZone() const;
    
 private:
    ZoneName m_name;
    class Private;
    Private* d;
};
};

#endif
