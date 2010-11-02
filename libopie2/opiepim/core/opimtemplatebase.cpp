/*
                             This file is part of the Opie Project
                             Copyright (C) 2004 Holger Freyther <zecke@handhelds.org>
              =.             Copyright (C) The Opie Team <opie-devel@lists.sourceforge.net>
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

#include "opimtemplatebase.h"

#include <opie2/opimoccurrence.h>
#include <opie2/private/opimoccurrence_p.h>

namespace Opie {

static void setPeriod( OPimOccurrence& oc, bool period, const QDate& d,
                       const QTime& s, const QTime& t ) {
    if ( period )
        oc.setPeriod( d );
    else
        oc.setPeriod( d, s, t );
}

// namespace Opie {
OPimBase::OPimBase() : d( 0 ) {}
OPimBase::~OPimBase() {}

/**
 * @internal Convert internal Occurrence representation
 * to the external
 */
OPimOccurrence::List OPimBase::convertOccurrenceFromBackend( const OPimBackendOccurrence::List& lst )const {
    OPimOccurrence::List oc_lst;

    /*
     * Split multiday events up. Create the internal data structure
     * and then iterate over the days and create the OPimOccurrecne.
     */
    for ( OPimBackendOccurrence::List::ConstIterator it = lst.begin(); it != lst.end(); ++it ) {
        OPimBackendOccurrence boc = *it;

        /*
         * Create the Shared Data Structure
         */
        OPimOccurrence::Data *data = new OPimOccurrence::Data();
        data->summary = boc.summary();
        data->location = boc.location();
        data->note = boc.note();
        data->uid = boc.uid();
        data->backend = const_cast<OPimBase*>(this);

        QDateTime start = boc.startDateTime();
        QDateTime end = boc.endDateTime();

        /*
         * Start and End are on the same day
         * Start and End are on two different ways.
         *      - Add Start and End and the days inbetween
         */
        int dto = start.daysTo( end );
        bool allDay = boc.isAllDay();

        if ( dto == 0 ) {
            OPimOccurrence oc = OPimOccurrence( data, OPimOccurrence::StartEnd );
            setPeriod( oc, allDay, start.date(), start.time(), end.time() );
            oc_lst.append( oc );
        }else {

            OPimOccurrence oc = OPimOccurrence( data, OPimOccurrence::Start );
            setPeriod( oc, allDay, start.date(), start.time(), QTime(23,59,59));
            oc_lst.append( oc );

            QDate next = start.addDays( 1 ).date();
            while ( next != end.date() ) {
                oc = OPimOccurrence( data, OPimOccurrence::MidWay );
                setPeriod( oc, allDay, next, QTime(0, 0, 0), QTime(23, 59, 59));
                oc_lst.append( oc );
                next = next.addDays( 1 );
            }

            oc = OPimOccurrence( data, OPimOccurrence::End );
            setPeriod( oc, allDay, end.date(), QTime(0, 0, 0 ), end.time() );
            oc_lst.append( oc );
        }
    }

    return oc_lst;
}
// }

}
