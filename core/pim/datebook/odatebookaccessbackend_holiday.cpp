/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers (Eilers.Stefan@epost.de)
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

/* OPIE */
#include "odatebookaccessbackend_holiday.h"
#include <opie2/odebug.h>

using namespace Opie;
using namespace Opie::Datebook;

ODateBookAccessBackend_Holiday::ODateBookAccessBackend_Holiday( HolidayPlugin2 *holidayplugin )
    : ODateBookAccessBackend(), plugin(holidayplugin) {
}

ODateBookAccessBackend_Holiday::~ODateBookAccessBackend_Holiday() {
}

bool ODateBookAccessBackend_Holiday::load() {
    return true;
}

bool ODateBookAccessBackend_Holiday::reload() {
    return true;
}

bool ODateBookAccessBackend_Holiday::save() {
    return false;
}

QArray<int> ODateBookAccessBackend_Holiday::allRecords()const {
    return QArray<int>();
}

QArray<int> ODateBookAccessBackend_Holiday::queryByExample(const OPimEvent&, int, const QDateTime& ) const {
    return QArray<int>();
}

void ODateBookAccessBackend_Holiday::clear() {
}

OPimEvent ODateBookAccessBackend_Holiday::find( int ) const {
    OPimEvent ev(0);
    ev.setAllDay(true);
    return ev;
}

bool ODateBookAccessBackend_Holiday::add( const OPimEvent& ) {
    return false;
}

bool ODateBookAccessBackend_Holiday::remove( int ) {
    return false;
}

bool ODateBookAccessBackend_Holiday::replace( const OPimEvent& ) {
    return false;
}

QArray<int> ODateBookAccessBackend_Holiday::rawRepeats()const {
    return QArray<int>();
}

QArray<int> ODateBookAccessBackend_Holiday::nonRepeats()const {
    return QArray<int>();
}

OPimEvent::ValueList ODateBookAccessBackend_Holiday::directNonRepeats()const {
    return OPimEvent::ValueList();
}

OPimEvent::ValueList ODateBookAccessBackend_Holiday::directRawRepeats()const {
    return OPimEvent::ValueList();
}

QArray<int> ODateBookAccessBackend_Holiday::matchRegexp(  const QRegExp & ) const
{
    return QArray<int>();
}

OPimBackendOccurrence::List ODateBookAccessBackend_Holiday::occurrences( const QDate& from,
                                                                 const QDate& to )const {
    return effectiveNonRepeatingEvents( from, to );
}

OPimBackendOccurrence::List ODateBookAccessBackend_Holiday::effectiveNonRepeatingEvents( const QDate& from,
                                                                                 const QDate& to )const
{
    OPimBackendOccurrence::List boc_lst;
    QMap<QDate,QStringList> items;

    QDate d = (from<to?from:to);
    int daysto = from.daysTo(to);
    if (daysto < 0)
        daysto = to.daysTo(from);

    items = plugin->entries( from, to );

    QStringList temp;
    for (int i =0; i<= daysto;++i) {
        QDate date = d.addDays(i);
        temp = items[date];
        if (temp.count()==0) {
            continue;
        }
        for (unsigned j=0;j<temp.count();++j) {
            OPimBackendOccurrence boc(date, -1);
            boc.setSummary(temp[j]);
            boc_lst.append( boc );
        }
    }

    return boc_lst;
}
