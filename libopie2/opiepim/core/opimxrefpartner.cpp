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
#include "opimxrefpartner.h"

namespace Opie {

OPimXRefPartner::OPimXRefPartner( const QString& appName,
                                  int uid, int field )
    : m_app(appName), m_uid(uid), m_field( field ) {
}

OPimXRefPartner::OPimXRefPartner( const OPimXRefPartner& ref ) {
    *this = ref;
}

OPimXRefPartner::~OPimXRefPartner() {
}

OPimXRefPartner &OPimXRefPartner::operator=( const OPimXRefPartner& par ) {
    m_app = par.m_app;
    m_uid = par.m_uid;
    m_field = par.m_field;

    return *this;
}

bool OPimXRefPartner::operator==( const OPimXRefPartner& par ) {
    if ( m_app != par.m_app ) return false;
    if ( m_uid != par.m_uid ) return false;
    if ( m_field != par.m_field ) return false;

    return true;
}

QString OPimXRefPartner::service()const {
    return m_app;
}

int OPimXRefPartner::uid()const {
    return m_uid;
}

int OPimXRefPartner::field()const {
    return m_field;
}

void OPimXRefPartner::setService( const QString& appName ) {
    m_app = appName;
}

void OPimXRefPartner::setUid( int uid ) {
    m_uid = uid;
}

void OPimXRefPartner::setField( int field ) {
    m_field = field;
}

}
