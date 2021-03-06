/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#include "fontfactoryttf_qws.h"
#include "freetypefactoryimpl.h"


FreeTypeFactoryImpl::FreeTypeFactoryImpl()
    : factory(0)
{
}

FreeTypeFactoryImpl::~FreeTypeFactoryImpl()
{
    delete factory;
}

QFontFactory *FreeTypeFactoryImpl::fontFactory()
{
    if ( !factory )
	factory = new QFontFactoryFT();
    return factory;
}

QRESULT FreeTypeFactoryImpl::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
{
    *iface = 0;
    if ( uuid == IID_QUnknown )
	*iface = this;
    else if ( uuid == IID_FontFactory )
	*iface = this;

    if ( *iface )
	(*iface)->addRef();
    return (*iface) ? QS_OK : QS_FALSE;
}

Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( FreeTypeFactoryImpl )
}
