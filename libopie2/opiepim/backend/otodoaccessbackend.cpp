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

#include <opie2/otodoaccessbackend.h>
#include <opie2/private/opimtodosortvector.h>
#include <opie2/otodoaccess.h>

#include <qintdict.h>

namespace Opie {
OPimTodoAccessBackend::OPimTodoAccessBackend()
    : OPimAccessBackend<OPimTodo>()
{
}
OPimTodoAccessBackend::~OPimTodoAccessBackend() {

}

const uint OPimTodoAccessBackend::querySettings() const
{
	return 0;
}

bool OPimTodoAccessBackend::hasQuerySettings (uint querySettings) const
{
	return false;
}


UIDArray OPimTodoAccessBackend::queryByExample( const UIDArray& uidlist, const OPimTodo& query, int settings,
                                                 const QDateTime& endperiod )const 
{
	qDebug( "Accessing OPimTodoAccessBackend::queryByExample() which is not implemented!" );
	return UIDArray();
    
//     odebug << "Using Unaccelerated OPimContactAccessBackend implementation of queryByExample!" << oendl;

//     UIDArray m_currentQuery( uid_array.count() );
//     uint arraycounter = 0;

//     for( uint it = 0; it < uid_array.count(); ++it ){
// 	    /* Search all fields and compare them with query object. Store them into list
// 	     * if all fields matches.
// 	     */
//     }


}

UIDArray OPimTodoAccessBackend::sorted( const UIDArray& events, bool asc,
                                        int sortOrder, int sortFilter,
                                        const QArray<int>& categories )const {
    odebug << "Using Unaccelerated TodoList sorted Implementation" << oendl;
    Internal::OPimTodoSortVector vector(events.count(), asc,sortOrder );
    int item = 0;

    bool bCat = sortFilter  & OPimTodoAccess::FilterCategory ? true : false;
    bool bOnly = sortFilter & OPimTodoAccess::OnlyOverDue ? true : false;
    bool comp = sortFilter  & OPimTodoAccess::DoNotShowCompleted ? true : false;
    bool catPassed = false;
    int cat;

    for ( uint i = 0; i < events.count(); ++i ) {
        OPimTodo todo = find( events[i], events, i, Frontend::Forward );
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
        if ( !todo.isOverdue() && bOnly )
            continue;
        if (todo.isCompleted() && comp )
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

OPimBackendOccurrence::List OPimTodoAccessBackend::occurrences( const QDate& start,
                                                                const QDate& end )const {
    OPimBackendOccurrence::List lst;
    UIDArray effective = effectiveToDos( start, end, false );
    UIDArray overdue = overDue();
    uint count = effective.count();
    int uid;
    QIntDict<int> hash;
    hash.setAutoDelete( true );
    OPimTodo todo;

    for ( uint i = 0; i < count; ++i ) {
        uid = effective[i];
        todo = find( uid, effective, i, Frontend::Forward );
        /*
         * If isOverdue but in the 'normal' range we will fill
         * the hash so we won't have duplicates in OPimBackendOccurrence
         */
        if ( todo.isOverdue() )
            hash.insert( uid, new int(6) );
        OPimBackendOccurrence oc = todo.hasStartDate() ?
                                   OPimBackendOccurrence( todo.startDate(),
                                                          todo.dueDate(), uid ) :
                                   OPimBackendOccurrence( todo.dueDate(), uid, QString::null );
        oc.setSummary( todo.summary() );
        lst.append( oc );
    }

    /*
     * Create the OverDue items but skip
     * the already handled Records
     */
    if ( !overdue.isEmpty() ) {
        QDate today = QDate::currentDate();
        QDate dueDate = (start >= today && today <= end ) ? today : start;
        count = overdue.count();
        for ( uint i = 0; i < count; ++i ) {
            uid = overdue[i];
            if (!hash.find( uid ) )
                continue;
            todo = find( uid, overdue, i, Frontend::Forward );
            lst.append( OPimBackendOccurrence(dueDate, uid, todo.summary() ) );
        }
    }

    return lst;
}
}
