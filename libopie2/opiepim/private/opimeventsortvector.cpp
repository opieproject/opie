/*
                             This file is part of the Opie Project
                             Copyright (C) 2004 Stefan Eilers <stefan@eilers-online.net>
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

#include "opimeventsortvector.h"
#include <opie2/ocontactaccess.h>
#include <opie2/opimnotifymanager.h>
#include <opie2/odatebookaccess.h>

#include <qvaluelist.h>

namespace Opie {
namespace Internal {

int testAlarmNotifiers( const OPimNotifyManager& leftnotifiers, const OPimNotifyManager& rightnotifiers ){
	OPimNotifyManager::Alarms left_alarms = leftnotifiers.alarms();
	OPimNotifyManager::Alarms right_alarms = rightnotifiers.alarms();

	// Well.. How could we compare two lists of alarms? I think we should find the most early datetimes
	// and compare them.. (se)
	// Find the first alarm of the left list
	OPimNotifyManager::Alarms::Iterator it;
	QDateTime left_earliest; // This datetime is initialized as invalid!!
	for ( it = left_alarms.begin(); it != left_alarms.end(); ++it ){
		if ( !left_earliest.isValid() || left_earliest > (*it).dateTime() ){
			left_earliest = (*it).dateTime();
		}
	}
	QDateTime right_earliest; // This datetime is initialized as invalid!!
	for ( it = right_alarms.begin(); it != right_alarms.end(); ++it ){
		if ( !right_earliest.isValid() || right_earliest > (*it).dateTime() ){
			right_earliest = (*it).dateTime();
		}
	}

	int ret = 0;

	// Now compare this found alarms
	if ( !left_earliest .isValid() ) ret++;
	if ( !right_earliest.isValid() ) ret--;

	if ( left_earliest.isValid() && right_earliest.isValid() ){
		left_earliest < right_earliest ? ret-- : ret++;
	}

	return ret;

}

OPimEventSortVector::OPimEventSortVector( uint size, bool asc, int sort )
    : OPimSortVector<OPimEvent>( size, asc, sort ) {}

int OPimEventSortVector::compareItems( const OPimEvent& left,
                                         const OPimEvent& right ) {
    if ( left.uid() == right.uid() )
        return 0;

    int ret = 0;
    bool asc = sortAscending();

    switch( sortOrder() ) {
    case ODateBookAccess::SortDescription:
        ret = testString( left.description(), right.description() );
        break;
    case ODateBookAccess::SortLocation:
        ret = testString( left.location(), right.location() );
        break;
    case ODateBookAccess::SortNote:
        ret = testString( left.note(),right.note() );
        break;
    case ODateBookAccess::SortStartTime:
	    ret = testTime( left.startDateTime().time(), right.startDateTime().time() );
        break;
    case ODateBookAccess::SortEndTime:
	    ret = testTime( left.endDateTime().time(), right.endDateTime().time() );
        break;
    case ODateBookAccess::SortStartDate:
	    ret = testDate( left.startDateTime().date(), right.startDateTime().date() );
        break;
    case ODateBookAccess::SortEndDate:
	    ret = testDate( left.endDateTime().date(), right.endDateTime().date() );
        break;
    case ODateBookAccess::SortStartDateTime:
	    ret = testDateTime( left.startDateTime(), right.startDateTime() );
        break;
    case ODateBookAccess::SortEndDateTime:
	    ret = testDateTime( left.endDateTime(), right.endDateTime() );
        break;
    case ODateBookAccess::SortAlarmDateTime:
	    ret = testAlarmNotifiers( left.notifiers(), right.notifiers() );
        break;
    default:
	    odebug << "OpimEventSortVector: Unknown sortOrder: " << sortOrder() << oendl;
    }

    /* twist to honor ascending/descending setting as QVector only sorts ascending */
    if ( !asc )
        ret *= -1;

    //  Maybe differentiate as in OPimTodoSortVector ### FIXME
    //  if( ret )
    return ret;
}

}
}
