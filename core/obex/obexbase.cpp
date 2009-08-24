/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Maximilian Reiss <max.reiss@gmx.de>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <,   >  .   <=           redistribute it and/or  modify it under
:=1 )Y*s>-.--   :            the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the License.
     ._= =}       :
    .%+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|     MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>:      PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .     .:        details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=            this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/
/*
 * The basic class for OBEX manipulating classes implementation
 */

#include "obexbase.h"

/* OPIE */
#include <opie2/oprocess.h>
#include <opie2/odebug.h>

using namespace  OpieObex;

using namespace Opie::Core;

ObexBase::ObexBase(QObject *parent, const char* name)
    : QObject(parent, name)
{
    m_count = 0;
    m_receive = false;
    connect( this, SIGNAL(error(int) ), // for recovering to receive
             SLOT(slotError() ) );
    connect( this, SIGNAL(sent(bool) ),
             SLOT(slotError() ) );
}

ObexBase::~ObexBase() {
}

void ObexBase::receive()  {
    m_receive = true;
    m_outp = QString::null;
}

void ObexBase::send( const QString& fileName, const QString& bdaddr) {
    // if currently receiving stop it send receive
    m_count = 0;
    m_file = fileName;
    m_bdaddr = bdaddr;
}

void ObexBase::setReceiveEnabled(bool) {
}

void ObexBase::slotError() {
}

//eof
