/*
                             This file is part of the Opie Project
                             Copyright (C) 2004 Holger Freyther <freyther@handhelds.org>
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

#include "ocontactaccessbackend.h"
#include <opie2/private/opimcontactsortvector.h>
#include <opie2/ocontactaccess.h>

#include <opie2/odebug.h>

#include <qdatetime.h>

namespace Opie {
OPimContactAccessBackend::OPimContactAccessBackend() {}

UIDArray OPimContactAccessBackend::queryByExample( const UIDArray& uid_array, const OPimContact& query, int settings,
						   const QDateTime& d )const {
	odebug << "Using Unaccelerated OPimContactAccessBackend implementation of queryByExample!" << oendl;

	UIDArray m_currentQuery( uid_array.count() );
	uint arraycounter = 0;

	for( uint it = 0; it < uid_array.count(); ++it ){
		/* Search all fields and compare them with query object. Store them into list
		 * if all fields matches.
		 */
		QDate* queryDate = 0l;
		QDate* checkDate = 0l;
		bool allcorrect = true;
		for ( int i = 0; i < Qtopia::Groups; i++ ) {
			// Birthday and anniversary are special nonstring fields and should
			// be handled specially
			switch ( i ){
			case Qtopia::Birthday:
				queryDate = new QDate( query.birthday() );
				checkDate = new QDate( find( uid_array[it] ).birthday() );
				// fall through
			case Qtopia::Anniversary:
				if ( queryDate == 0l ){
					queryDate = new QDate( query.anniversary() );
					checkDate = new QDate( find( uid_array[it] ).anniversary() );
				}

				if ( queryDate->isValid() ){
					if(  checkDate->isValid() ){
						if ( settings & OPimContactAccess::DateYear ){
							if ( queryDate->year() != checkDate->year() )
								allcorrect = false;
						}
						if ( settings & OPimContactAccess::DateMonth ){
							if ( queryDate->month() != checkDate->month() )
								allcorrect = false;
						}
						if ( settings & OPimContactAccess::DateDay ){
							if ( queryDate->day() != checkDate->day() )
								allcorrect = false;
						}
						if ( settings & OPimContactAccess::DateDiff ) {
							QDate current;
							// If we get an additional date, we
							// will take this date instead of
							// the current one..
							if ( !d.date().isValid() )
								current = QDate::currentDate();
							else
								current = d.date();

							// We have to equalize the year, otherwise
							// the search will fail..
							checkDate->setYMD( current.year(),
									   checkDate->month(),
									   checkDate->day() );
							if ( *checkDate < current )
								checkDate->setYMD( current.year()+1,
										   checkDate->month(),
										   checkDate->day() );

							// Check whether the birthday/anniversary date is between
							// the current/given date and the maximum date
							// ( maximum time range ) !
							if ( current.daysTo( *queryDate ) >= 0 ){
								if ( !( ( *checkDate >= current ) &&
									( *checkDate <= *queryDate ) ) ){
									allcorrect = false;
								}
							}
						}
					} else{
						// checkDate is invalid. Therefore this entry is always rejected
						allcorrect = false;
					}
				}

				delete queryDate;
				queryDate = 0l;
				delete checkDate;
				checkDate = 0l;
				break;
			default:
				/* Just compare fields which are not empty in the query object */
				if ( !query.field(i).isEmpty() ){
					switch ( settings & ~( OPimContactAccess::IgnoreCase
							       | OPimContactAccess::DateDiff
							       | OPimContactAccess::DateYear
							       | OPimContactAccess::DateMonth
							       | OPimContactAccess::DateDay
							       | OPimContactAccess::MatchOne
							       ) ){

					case OPimContactAccess::RegExp:{
						QRegExp expr ( query.field(i),
							       !(settings & OPimContactAccess::IgnoreCase),
							       false );
						if ( expr.find ( find( uid_array[it] ).field(i), 0 ) == -1 )
							allcorrect = false;
					}
						break;
					case OPimContactAccess::WildCards:{
						QRegExp expr ( query.field(i),
							       !(settings & OPimContactAccess::IgnoreCase),
							       true );
						if ( expr.find ( find( uid_array[it] ).field(i), 0 ) == -1 )
							allcorrect = false;
					}
						break;
					case OPimContactAccess::ExactMatch:{
						if (settings & OPimContactAccess::IgnoreCase){
							if ( query.field(i).upper() !=
							     find( uid_array[it] ).field(i).upper() )
								allcorrect = false;
						}else{
							if ( query.field(i) != find( uid_array[it] ).field(i) )
								allcorrect = false;
						}
					}
						break;
					}
				}
			}
		}
		if ( allcorrect ){
			m_currentQuery[arraycounter++] = uid_array[it];
		}
	}

	// Shrink to fit..
	m_currentQuery.resize(arraycounter);

	return m_currentQuery;

}

const uint OPimContactAccessBackend::querySettings() const
{
    return ( OPimContactAccess::WildCards
         | OPimContactAccess::IgnoreCase
         | OPimContactAccess::RegExp
         | OPimContactAccess::ExactMatch
         | OPimContactAccess::DateDiff
         | OPimContactAccess::DateYear
         | OPimContactAccess::DateMonth
         | OPimContactAccess::DateDay
         );
}

bool OPimContactAccessBackend::hasQuerySettings (uint querySettings) const
{
    /* OPimContactAccess::IgnoreCase, DateDiff, DateYear, DateMonth, DateDay
     * may be added with any of the other settings. IgnoreCase should never used alone.
     * Wildcards, RegExp, ExactMatch should never used at the same time...
     */

    // Step 1: Check whether the given settings are supported by this backend
    if ( ( querySettings & (
                OPimContactAccess::IgnoreCase
                | OPimContactAccess::WildCards
                | OPimContactAccess::DateDiff
                | OPimContactAccess::DateYear
                | OPimContactAccess::DateMonth
                | OPimContactAccess::DateDay
                | OPimContactAccess::RegExp
                | OPimContactAccess::ExactMatch
                   ) ) != querySettings )
        return false;

    // Step 2: Check whether the given combinations are ok..

    // IngoreCase alone is invalid
    if ( querySettings == OPimContactAccess::IgnoreCase )
        return false;

    // WildCards, RegExp and ExactMatch should never used at the same time
    switch ( querySettings & ~( OPimContactAccess::IgnoreCase
                    | OPimContactAccess::DateDiff
                    | OPimContactAccess::DateYear
                    | OPimContactAccess::DateMonth
                    | OPimContactAccess::DateDay
                    )
         ){
    case OPimContactAccess::RegExp:
        return ( true );
    case OPimContactAccess::WildCards:
        return ( true );
    case OPimContactAccess::ExactMatch:
        return ( true );
    case 0: // one of the upper removed bits were set..
        return ( true );
    default:
        return ( false );
    }
}


UIDArray OPimContactAccessBackend::sorted( const UIDArray& ar, bool asc, int sortOrder,
                                  int filter, const QArray<int>& categories)const {
    odebug << "Using Unaccelerated OPimContactAccessBackend sorted Implementation" << oendl;

    Internal::OPimContactSortVector vector(ar.count(), asc, sortOrder );

    int item = 0;
    uint cat_count = categories.count();
    uint eve_count = ar.count();
    bool bCat = filter  & OPimContactAccess::FilterCategory ? true : false;
    bool catPassed = false;
    int cat;

    for ( uint i = 0; i < eve_count; ++i ) {
        OPimContact contact = find( ar[i], ar, i, Frontend::Forward );
        if ( contact.isEmpty() )
            continue;

        /* show category */
        /* -1 == unfiled */
        catPassed = false;
        for ( uint cat_nu = 0; cat_nu < cat_count; ++cat_nu ) {
            cat = categories[cat_nu];
            if ( bCat && cat == -1 ) {
                if(!contact.categories().isEmpty() )
                    continue;
            } else if ( bCat && cat != 0)
                if (!contact.categories().contains( cat ) )
                    continue;
            catPassed = true;
            break;
        }

        /*
         * If none of the Categories matched
         * continue
         */
        if ( !catPassed )
            continue;

        vector.insert(item++, contact );
    }

    vector.resize( item );
    /* sort it now */
    vector.sort();
    /* now get the uids */
    UIDArray array( vector.count() );
    for (uint i= 0; i < vector.count(); i++ )
        array[i] = vector.uidAt( i );

    return array;
}

OPimBackendOccurrence::List OPimContactAccessBackend::occurrences( const QDate& start,
                                                                   const QDate& end)const {
    OPimBackendOccurrence::List lst;

    UIDArray records = allRecords();
    const uint count = records.count();
    int uid;

    for ( uint i = 0; i < count; ++i ) {
        uid = records[i];
        OPimContact contact = find(uid, records, i, Frontend::Forward );

        QDate date = contact.anniversary();
        date = QDate( start.year(), date.month(),date.day() );

//        if ( date.isValid() && date.) {
//        }
    }

    return lst;
}
}
