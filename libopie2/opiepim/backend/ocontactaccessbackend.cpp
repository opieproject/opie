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

namespace Opie {
OPimContactAccessBackend::OPimContactAccessBackend() {}

UIDArray
OPimContactAccessBackend::queryByExample( const OPimContact&, int,
                                          const QDateTime& )const {
    return UIDArray();
}

UIDArray
OPimContactAccessBackend::sorted( const UIDArray& ar, bool asc, int sortOrder,
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
