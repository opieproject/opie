/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Maximilian Reiss <max.reiss@gmx.de>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <,   >  .   <=          redistribute it and/or  modify it under
:=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "bluezapplet.h"
#include "bluezappletimpl.h"

namespace OpieTooth {

    BluezAppletImpl::BluezAppletImpl() : bluetooth(0),  ref(0) {
    }

    BluezAppletImpl::~BluezAppletImpl() {
        delete bluetooth;
    }

    QWidget *BluezAppletImpl::applet( QWidget *parent ) {
        if ( !bluetooth ) {
            bluetooth = new BluezApplet( parent );
        }
        return bluetooth;
    }

    int BluezAppletImpl::position() const {
        return 6;
    }

    QRESULT BluezAppletImpl::queryInterface( const QUuid &uuid, QUnknownInterface **iface ) {
        *iface = 0;
        if ( uuid == IID_QUnknown ) {
            *iface = this;
        } else if ( uuid == IID_TaskbarApplet ) {
            *iface = this;
        }

        if ( *iface ) {
            (*iface)->addRef();
        }
        return QS_OK;
    }

    Q_EXPORT_INTERFACE() {
        Q_CREATE_INSTANCE( BluezAppletImpl )
            }

};


