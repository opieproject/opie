/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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
#include "libflashplugin.h"
#include "libflashpluginimpl.h"


LibFlashPluginImpl::LibFlashPluginImpl()
    : libflashplugin(0), ref(0)
{
}


LibFlashPluginImpl::~LibFlashPluginImpl()
{
    if ( libflashplugin )
	delete libflashplugin;
}


MediaPlayerDecoder *LibFlashPluginImpl::decoder()
{
    if ( !libflashplugin )
	libflashplugin = new LibFlashPlugin;
    return libflashplugin;
}


MediaPlayerEncoder *LibFlashPluginImpl::encoder()
{
    return NULL;
}


#ifndef QT_NO_COMPONENT


QRESULT LibFlashPluginImpl::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
{
    *iface = 0;
    if ( ( uuid == IID_QUnknown ) || ( uuid == IID_MediaPlayerPlugin ) )
	*iface = this, (*iface)->addRef();
    return QS_OK;
}


Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( LibFlashPluginImpl )
}


#endif

