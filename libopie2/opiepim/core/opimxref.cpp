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
#include <opie2/opimxref.h>

namespace Opie {

OPimXRef::OPimXRef( const OPimXRefPartner& one, const OPimXRefPartner& two )
    : m_partners(2)
{
    m_partners[0] = one;
    m_partners[1] = two;
}
OPimXRef::OPimXRef()
    : m_partners(2)
{

}
OPimXRef::OPimXRef( const OPimXRef& ref) {
    *this = ref;
}
OPimXRef::~OPimXRef() {
}
OPimXRef &OPimXRef::operator=( const OPimXRef& ref) {
    m_partners = ref.m_partners;
    m_partners.detach();

    return* this;
}
bool OPimXRef::operator==( const OPimXRef& oper ) {
    if ( m_partners == oper.m_partners ) return true;

    return false;
}
OPimXRefPartner OPimXRef::partner( enum Partners par) const{
    return m_partners[par];
}
void OPimXRef::setPartner( enum Partners par,  const OPimXRefPartner& part) {
    m_partners[par] = part;
}
bool OPimXRef::containsString( const QString& string ) const{
    if ( m_partners[One].service() == string ||
         m_partners[Two].service() == string ) return true;

    return false;
}
bool OPimXRef::containsUid( int uid ) const{
    if ( m_partners[One].uid() == uid ||
         m_partners[Two].uid() == uid ) return true;

    return false;
}

}
