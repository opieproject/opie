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
#ifndef OPIE_DATE_BOOK_ACCESS_H
#define OPIE_DATE_BOOK_ACCESS_H

#include "odatebookaccessbackend.h"
#include "opimaccesstemplate.h"

#include <opie2/oevent.h>

namespace Opie {
/**
 * This is the object orientated datebook database. It'll use OBackendFactory
 * to query for a backend.
 * All access to the datebook should be done via this class.
 * Make sure to load and save the datebook this is not part of
 * destructing and creating the object
 *
 * @author Holger Freyther, Stefan Eilers
 */
class ODateBookAccess : public OPimAccessTemplate<OEvent> {
public:
    ODateBookAccess( ODateBookAccessBackend* = 0l, enum Access acc = Random );
    ~ODateBookAccess();

    /* return all events */
    List rawEvents()const;

    /* return repeating events */
    List rawRepeats()const;

    /* return non repeating events */
    List nonRepeats()const;

    /* return non repeating events (from,to) */
    OEffectiveEvent::ValueList effectiveEvents( const QDate& from, const QDate& to ) const;
    OEffectiveEvent::ValueList effectiveEvents( const QDateTime& start ) const;
    OEffectiveEvent::ValueList effectiveNonRepeatingEvents( const QDate& from, const QDate& to ) const;
    OEffectiveEvent::ValueList effectiveNonRepeatingEvents( const QDateTime& start ) const;

private:
    ODateBookAccessBackend* m_backEnd;
    class Private;
    Private* d;
};

}

#endif
