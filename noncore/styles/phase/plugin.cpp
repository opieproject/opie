/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Holger Hans Peter Freyther <zecke@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this library; see the file COPYING.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "plugin.h"

#include "phasestyle.h"

#include <qapplication.h>

PhaseStyleImpl::PhaseStyleImpl()
    : m_style( 0l )
{}

/* Qt will delete the style */
PhaseStyleImpl::~PhaseStyleImpl()
{}

QRESULT PhaseStyleImpl::queryInterface( const QUuid& uuid, QUnknownInterface **iface ) {
    *iface = 0;

    if ( uuid == IID_QUnknown || uuid == IID_Style)
        *iface = this;
    else
        return QS_FALSE;

    (*iface)->addRef();

    return QS_OK;
}

QStyle* PhaseStyleImpl::style() {
    if (!m_style )
        m_style = new PhaseStyle();
    return m_style;
}

QString PhaseStyleImpl::name()const {
    return qApp->translate("PhaseStyle", "Phase", "Name of the style Phase");
}


Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( PhaseStyleImpl )
}

