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
#include "opimxrefmanager.h"

namespace Opie {

OPimXRefManager::OPimXRefManager() {
}

OPimXRefManager::OPimXRefManager( const OPimXRefManager& ref) {
    m_list = ref.m_list;
}

OPimXRefManager::~OPimXRefManager() {
}

OPimXRefManager &OPimXRefManager::operator=( const OPimXRefManager& ref) {
    m_list = ref.m_list;
    return *this;
}

bool OPimXRefManager::operator==( const OPimXRefManager& /*ref*/) {
    //   if ( m_list == ref.m_list ) return true;

    return false;
}

void OPimXRefManager::add( const OPimXRef& ref) {
    m_list.append( ref );
}

void OPimXRefManager::remove( const OPimXRef& ref) {
    m_list.remove( ref );
}

void OPimXRefManager::replace( const OPimXRef& ref) {
    m_list.remove( ref );
    m_list.append( ref );
}

void OPimXRefManager::clear() {
    m_list.clear();
}

QStringList OPimXRefManager::apps() const {
    OPimXRef::ValueList::ConstIterator it;
    QStringList list;

    QString str;
    for ( it = m_list.begin(); it != m_list.end(); ++it ) {
        str = (*it).partner( OPimXRef::One ).service();
        if ( !list.contains( str ) ) list << str;

        str = (*it).partner( OPimXRef::Two ).service();
        if ( !list.contains( str ) ) list << str;
    }
    return list;
}

OPimXRef::ValueList OPimXRefManager::list() const {
    return m_list;
}

OPimXRef::ValueList OPimXRefManager::list( const QString& appName ) const{
    OPimXRef::ValueList list;
    OPimXRef::ValueList::ConstIterator it;

    for ( it = m_list.begin(); it != m_list.end(); ++it ) {
        if ( (*it).containsString( appName ) )
            list.append( (*it) );
    }

    return list;
}

OPimXRef::ValueList OPimXRefManager::list( int uid ) const {
    OPimXRef::ValueList list;
    OPimXRef::ValueList::ConstIterator it;

    for ( it = m_list.begin(); it != m_list.end(); ++it ) {
        if ( (*it).containsUid( uid ) )
            list.append( (*it) );
    }

    return list;
}

}
