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
#include <opie2/obackendfactory.h>
#include <opie2/odatebookaccess.h>

namespace Opie {
/**
 * Simple constructor
 * It takes a ODateBookAccessBackend as parent. If it is 0 the default implementation
 * will be used!
 * @param back The backend to be used or 0 for the default backend
 * @param ac What kind of access is intended
 */
ODateBookAccess::ODateBookAccess( ODateBookAccessBackend* back, enum Access ac )
    : OPimAccessTemplate<OPimEvent>( back )
{
    if (!back )
        back = OBackendFactory<ODateBookAccessBackend>::Default("datebook", QString::null );

    m_backEnd = back;
    setBackEnd( m_backEnd );
}
ODateBookAccess::~ODateBookAccess() {
}

/**
 * @return all events available
 */
ODateBookAccess::List ODateBookAccess::rawEvents()const {
    QArray<int> ints = m_backEnd->rawEvents();

    List lis( ints, this );
    return lis;
}

/**
 * @return all repeating events
 */
ODateBookAccess::List ODateBookAccess::rawRepeats()const {
    QArray<int> ints = m_backEnd->rawRepeats();

    List lis( ints, this );
    return lis;
}

/**
 * @return all non repeating events
 */
ODateBookAccess::List ODateBookAccess::nonRepeats()const {
    QArray<int> ints = m_backEnd->nonRepeats();

    List lis( ints, this );
    return lis;
}

/**
 * @return dates in the time span between from and to
 * @param from Include all events from...
 * @param to Include all events to...
 */
OEffectiveEvent::ValueList ODateBookAccess::effectiveEvents( const QDate& from, const QDate& to ) const {
    return m_backEnd->effectiveEvents( from, to );
}
/**
 * @return all events at a given datetime
 */
OEffectiveEvent::ValueList ODateBookAccess::effectiveEvents( const QDateTime& start ) const {
    return m_backEnd->effectiveEvents( start );
}

/**
 * @return non repeating dates in the time span between from and to
 * @param from Include all events from...
 * @param to Include all events to...
 */
OEffectiveEvent::ValueList ODateBookAccess::effectiveNonRepeatingEvents( const QDate& from, const QDate& to ) const {
    return m_backEnd->effectiveNonRepeatingEvents( from, to );
}
/**
 * @return all non repeating events at a given datetime
 */
OEffectiveEvent::ValueList ODateBookAccess::effectiveNonRepeatingEvents( const QDateTime& start ) const {
    return m_backEnd->effectiveNonRepeatingEvents( start );
}

}
