/*
                             This file is part of the Opie Project
                             Copyright (C) Holger Freyther <freyther@handhelds.org>
                             Copyright (C) Stefan Eilers <stefan@eilers-online.net>
              =.             Copyright (C) 2009 The Opie Team <opie-devel@handhelds.org>
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

#include "omemoaccessbackend.h"
#include <opie2/private/opimmemosortvector.h>
#include <opie2/omemoaccess.h>

#include <opie2/odebug.h>

#include <qdatetime.h>

namespace Opie {
OPimMemoAccessBackend::OPimMemoAccessBackend() {}

UIDArray OPimMemoAccessBackend::queryByExample( const UIDArray& uid_array, const OPimMemo& query, int settings,
                           const QDateTime& d )const
{
    qDebug( "Accessing OPimMemoAccessBackend::queryByExample() which is not implemented!" );
    return UIDArray();
}

uint OPimMemoAccessBackend::querySettings() const
{
    return ( OPimMemoAccess::WildCards
         | OPimMemoAccess::IgnoreCase
         | OPimMemoAccess::RegExp
         | OPimMemoAccess::ExactMatch
         );
}

bool OPimMemoAccessBackend::hasQuerySettings (uint querySettings) const
{
    /* OPimMemoAccess::IgnoreCase, DateDiff, DateYear, DateMonth, DateDay
     * may be added with any of the other settings. IgnoreCase should never used alone.
     * Wildcards, RegExp, ExactMatch should never used at the same time...
     */

    // Step 1: Check whether the given settings are supported by this backend
    if ( ( querySettings & (
                OPimMemoAccess::IgnoreCase
                | OPimMemoAccess::WildCards
                | OPimMemoAccess::RegExp
                | OPimMemoAccess::ExactMatch
                   ) ) != querySettings )
        return false;

    // Step 2: Check whether the given combinations are ok..

    // IngoreCase alone is invalid
    if ( querySettings == OPimMemoAccess::IgnoreCase )
        return false;

    // WildCards, RegExp and ExactMatch should never used at the same time
    switch ( querySettings & ~( OPimMemoAccess::IgnoreCase ) ) {
        case OPimMemoAccess::RegExp:
            return ( true );
        case OPimMemoAccess::WildCards:
            return ( true );
        case OPimMemoAccess::ExactMatch:
            return ( true );
        case 0: // one of the upper removed bits were set..
            return ( true );
        default:
            return ( false );
    }
}


UIDArray OPimMemoAccessBackend::sorted( const UIDArray& events, bool asc,
                                        int sortOrder, int sortFilter,
                                        const QArray<int>& categories ) const
{
    odebug << "Using Unaccelerated TodoList sorted Implementation" << oendl;
    Internal::OPimMemoSortVector vector(events.count(), asc,sortOrder );
    int item = 0;

    bool bCat = sortFilter  & OPimMemoAccess::FilterCategory ? true : false;
    bool catPassed = false;
    int cat;

    for ( uint i = 0; i < events.count(); ++i ) {
        OPimMemo todo = find( events[i], events, i, Frontend::Forward );
        if ( todo.isEmpty() )
            continue;

        /* show category */
        /* -1 == unfiled */
        catPassed = false;
        for ( uint cat_nu = 0; cat_nu < categories.count(); ++cat_nu ) {
            cat = categories[cat_nu];
            if ( bCat && cat == -1 ) {
                if(!todo.categories().isEmpty() )
                    continue;
            } else if ( bCat && cat != 0)
                if (!todo.categories().contains( cat ) )
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

        vector.insert(item++, todo );
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

OPimBackendOccurrence::List OPimMemoAccessBackend::occurrences( const QDate& start,
                                                                   const QDate& end)const {
    OPimBackendOccurrence::List lst;

    UIDArray records = allRecords();
    const uint count = records.count();
    int uid;

    for ( uint i = 0; i < count; ++i ) {
        uid = records[i];
        OPimMemo memo = find(uid, records, i, Frontend::Forward );

        // FIXME you kind of have to actually do something here
        // (same in OPimContactAccessBackend)
    }

    return lst;
}
}
