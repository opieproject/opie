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
#include <qdatetime.h>

#include <qpe/alarmserver.h>

// #include "otodoaccesssql.h"
#include <opie2/otodoaccess.h>
#include <opie2/obackendfactory.h>

namespace Opie {
OTodoAccess::OTodoAccess( OTodoAccessBackend* end, enum Access )
    : QObject(), OPimAccessTemplate<OTodo>( end ),  m_todoBackEnd( end )
{
//     if (end == 0l )
//         m_todoBackEnd = new OTodoAccessBackendSQL( QString::null);

	// Zecke: Du musst hier noch fr das XML-Backend einen Appnamen bergeben !
        if (end == 0l )
		m_todoBackEnd = OBackendFactory<OTodoAccessBackend>::Default ("todo", QString::null);

    setBackEnd( m_todoBackEnd );
}
OTodoAccess::~OTodoAccess() {
//    qWarning("~OTodoAccess");
}
void OTodoAccess::mergeWith( const QValueList<OTodo>& list ) {
    QValueList<OTodo>::ConstIterator it;
    for ( it = list.begin(); it != list.end(); ++it ) {
        replace( (*it) );
    }
}
OTodoAccess::List OTodoAccess::effectiveToDos( const QDate& start,
                                               const QDate& end,
                                               bool includeNoDates ) {
    QArray<int> ints = m_todoBackEnd->effectiveToDos( start, end, includeNoDates );

    List lis( ints, this );
    return lis;
}
OTodoAccess::List OTodoAccess::effectiveToDos( const QDate& start,
                                               bool includeNoDates ) {
    return effectiveToDos( start, QDate::currentDate(),
                           includeNoDates );
}
OTodoAccess::List OTodoAccess::overDue() {
    List lis( m_todoBackEnd->overDue(), this );
    return lis;
}
/* sort order */
OTodoAccess::List OTodoAccess::sorted( bool ascending, int sort,int filter, int cat ) {
    QArray<int> ints = m_todoBackEnd->sorted( ascending, sort,
                                      filter, cat );
    OTodoAccess::List list( ints, this );
    return list;
}
void OTodoAccess::removeAllCompleted() {
    m_todoBackEnd->removeAllCompleted();
}
QBitArray OTodoAccess::backendSupport( const QString& ) const{
    return m_todoBackEnd->supports();
}
bool OTodoAccess::backendSupports( int attr,  const QString& ar) const{
    return backendSupport(ar).testBit( attr );
}

}
