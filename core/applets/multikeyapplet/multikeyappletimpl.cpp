/**********************************************************************
** Copyright (C) 2004 Anton Kachalov mouse@altlinux.ru
**  All rights reserved.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/
#include "multikey.h"
#include "multikeyappletimpl.h"

MultikeyAppletImpl::MultikeyAppletImpl()
    : kbd(0)
{
}

MultikeyAppletImpl::~MultikeyAppletImpl()
{
    delete kbd;
}

QWidget *MultikeyAppletImpl::applet( QWidget *parent )
{
    if ( !kbd )
	kbd = new Multikey(parent);
    return kbd;
}

int MultikeyAppletImpl::position() const
{
    return 10;
}

#ifndef QT_NO_COMPONENT
QRESULT MultikeyAppletImpl::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
{
    *iface = 0;
    if ( uuid == IID_QUnknown )
	*iface = this;
    else if ( uuid == IID_TaskbarApplet )
	*iface = this;
    else
	return QS_FALSE;

    if ( *iface )
	(*iface)->addRef();
    return QS_OK;
}

Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( MultikeyAppletImpl )
}
#endif
