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

#ifndef OPIMXREFMANAGER_H
#define OPIMXREFMANAGER_H

/* OPIE */
#include <opie2/opimxref.h>

/* QT */
#include <qstringlist.h>

namespace Opie {
/**
 * This is a simple manager for
 * OPimXRefs.
 * It allows addition, removing, replacing
 * clearing and 'querying' the XRef...
 */
class OPimXRefManager {
public:
    OPimXRefManager();
    OPimXRefManager( const OPimXRefManager& );
    ~OPimXRefManager();

    OPimXRefManager& operator=( const OPimXRefManager& );
    bool operator==( const OPimXRefManager& );

    void add( const OPimXRef& );
    void remove( const OPimXRef& );
    void replace( const OPimXRef& );

    void clear();

    /**
     * apps participating
     */
    QStringList apps()const;
    OPimXRef::ValueList list()const;
    OPimXRef::ValueList list( const QString& service )const;
    OPimXRef::ValueList list( int uid )const;

private:
    OPimXRef::ValueList m_list;
    class Private;
    Private *d;
};

}

#endif
