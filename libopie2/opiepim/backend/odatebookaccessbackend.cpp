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
#include <qtl.h>

#include <opie2/odebug.h>
#include <opie2/opimrecurrence.h>
#include <opie2/odatebookaccess.h>
#include <opie2/odatebookaccessbackend.h>
#include <opie2/private/opimeventsortvector.h>

using namespace Opie;

namespace {
/* a small helper to get all NonRepeating events for a range of time */
void events( OPimBackendOccurrence::List& tmpList,
             const OPimEvent::ValueList& events,
             const QDate& from, const QDate& to ) {
    QDateTime dtStart, dtEnd;

    for ( OPimEvent::ValueList::ConstIterator it = events.begin(); it != events.end(); ++it ) {
        dtStart = (*it).startDateTime();
        dtEnd   = (*it).endDateTime();

        /*
         * If in range
         */
        if (dtStart.date() <= to && dtEnd.date() >= from ) {
            if((*it).isAllDay()) {
                OPimBackendOccurrence eff( dtStart.date(), (*it).uid() );
                tmpList.append( eff );
            }
            else {
                OPimBackendOccurrence eff( dtStart, dtEnd, (*it).uid() );
                tmpList.append( eff );
            }
        }
    }
}

void repeat( OPimBackendOccurrence::List& tmpList, const OPimEvent::ValueList& list,
             const QDate& from, const QDate& to ) {
    QDate repeat;
    for ( OPimEvent::ValueList::ConstIterator it = list.begin(); it != list.end(); ++it ) {
        int dur = (*it).startDateTime().date().daysTo( (*it).endDateTime().date() );
        QDate itDate = from.addDays(-dur );
        OPimRecurrence rec = (*it).recurrence();
        if ( !rec.hasEndDate() || rec.endDate() > to ) {
            rec.setEndDate( to );
            rec.setHasEndDate( true );
        }

        QDateTime start, end;
        while (rec.nextOcurrence(itDate, repeat ) ) {
            if (repeat > to ) break;

            OPimEvent event = *it;
            if(event.isAllDay()) {
                OPimBackendOccurrence eff(repeat, event.uid() );
                tmpList.append( eff );
            }
            else {
                start = QDateTime( repeat, event.startDateTime().time() );
                end   = QDateTime( repeat.addDays(dur), event.endDateTime().time() );
                OPimBackendOccurrence eff(start, end, event.uid() );
                tmpList.append( eff );
            }
            itDate = repeat.addDays(1);
        }
    }
}
}

namespace Opie {

ODateBookAccessBackend::ODateBookAccessBackend()
    : OPimAccessBackend<OPimEvent>()
{

}
ODateBookAccessBackend::~ODateBookAccessBackend() {

}
OPimBackendOccurrence::List ODateBookAccessBackend::occurrences( const QDate& from,
                                                                 const QDate& to )const {
    OPimBackendOccurrence::List tmpList;

    events( tmpList, directNonRepeats(), from, to );
    repeat( tmpList, directRawRepeats(),from,to );

    return tmpList;
}

OPimBackendOccurrence::List ODateBookAccessBackend::occurrences( const QDateTime& dt )const
{
    OPimBackendOccurrence::List day = occurrences( dt.date(), dt.date() );

    return filterOccurrences( day, dt );
}

OPimBackendOccurrence::List ODateBookAccessBackend::effectiveNonRepeatingEvents( const QDate& from,
                                                                                 const QDate& to )const
{
    OPimBackendOccurrence::List tmpList;
    OPimEvent::ValueList list = directNonRepeats();

    events( tmpList, list, from, to );

    return tmpList;
}

OPimBackendOccurrence::List ODateBookAccessBackend::effectiveNonRepeatingEvents( const QDateTime& dt )const
{
    OPimBackendOccurrence::List day = effectiveNonRepeatingEvents( dt.date(), dt.date() );
    return filterOccurrences( day,dt );
}

uint ODateBookAccessBackend::querySettings() const
{
	return 0;
}

bool ODateBookAccessBackend::hasQuerySettings (uint querySettings) const
{
	return false;
}



UIDArray ODateBookAccessBackend::queryByExample( const UIDArray& uidlist, const OPimEvent&, int settings,
                                                 const QDateTime& d )const
{
	qDebug( "Accessing ODateBookAccessBackend::queryByExample() which is not implemented!" );
    return UIDArray();
}

UIDArray ODateBookAccessBackend::sorted( const UIDArray& ar, bool asc, int sortOrder, int filter, const QArray<int>& categories )const {
    odebug << "Using Unaccelerated ODateBookAccessBackend sorted Implementation" << oendl;

    Internal::OPimEventSortVector vector( ar.count(), asc, sortOrder );
    int item = 0;

    for ( uint i = 0; i < ar.count(); ++i ){
	    OPimEvent event = find( ar[i], ar, i, Frontend::Forward );
	    if ( event.isEmpty() )
		    continue;

	    bool catPassed = true;
	    if ( filter & ODateBookAccess::FilterCategory ){
		    catPassed = false;
		    // Filter Categories
		    for ( uint cat_idx = 0; cat_idx < categories.count(); ++cat_idx ){
			    int cat = categories[cat_idx];
			    if ( cat == -1 || cat == 0 ){
				    // Unfiled. Check next category if list is not empty.
				    // Else: take it as we will not filter unfiled events..
				    if ( !event.categories().isEmpty() )
					    continue;
				    else
					    catPassed = true;
			    } else {
				    if ( !event.categories().contains( cat ) )
					    continue;
				    else{
					    catPassed = true;
					    break;
				    }
			    }
		    }
	    }

	    // Continue to next event if the category filter removed this item
	    if ( !catPassed )
		    continue;

	    vector.insert( item++, event );
    }

    // Now sort the vector and return the list of UID's
    vector.resize( item );
    vector.sort();

    UIDArray array( vector.count() );
    for ( uint i= 0; i < vector.count(); i++ )
        array[i] = vector.uidAt( i );

    return array;

}

OPimBackendOccurrence::List ODateBookAccessBackend::filterOccurrences( const OPimBackendOccurrence::List dayList,
                                                                       const QDateTime& dt ) {
    OPimBackendOccurrence::List tmpList;
    OPimBackendOccurrence::List::ConstIterator it;

    for ( it = dayList.begin(); it != dayList.end(); ++it ) {
        OPimBackendOccurrence occ = *it;

        /*
         * Let us find occurrences that are 'now'!
         * If the dt.date() is on the same day as start or end of the Occurrence
         * check how near start/end are.
         * If it is in the middle of a multiday occurrence list it.
         *
         * We might want to 'lose' the sixty second offset and list
         * all Events which are active at that time.
         */
        if ( dt.date() == occ.startDateTime().date() ) {
            if ( QABS( dt.time().secsTo( occ.startDateTime().time() ) ) < 60 )
                tmpList.append( occ );
        }else if ( dt.date() == occ.endDateTime().date() ) {
            if ( QABS( dt.time().secsTo( occ.endDateTime().time()   ) ) < 60 )
                tmpList.append( occ );
        }else if ( dt.date() >= occ.startDateTime().date() &&
                   dt.date() >= occ.endDateTime().date()  )
            tmpList.append( occ );
    }

    return tmpList;
}
}
