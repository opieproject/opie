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
#include "libmpeg3plugin.h"
#include "libmpeg3pluginimpl.h"


LibMpeg3PluginImpl::LibMpeg3PluginImpl()
    : libmpeg3plugin(0), ref(0)
{
}


LibMpeg3PluginImpl::~LibMpeg3PluginImpl()
{
    if ( libmpeg3plugin )
  delete libmpeg3plugin;
}


MediaPlayerDecoder *LibMpeg3PluginImpl::decoder()
{
    if ( !libmpeg3plugin )
  libmpeg3plugin = new LibMpeg3Plugin;
    return libmpeg3plugin;
}


MediaPlayerEncoder *LibMpeg3PluginImpl::encoder()
{
    return NULL;
}


#ifndef QT_NO_COMPONENT


QRESULT LibMpeg3PluginImpl::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
{
    *iface = 0;
    if ( ( uuid == IID_QUnknown )  || ( uuid == IID_MediaPlayerPlugin )  )
  *iface = this, (*iface)->addRef();
    return QS_OK;
}


Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( LibMpeg3PluginImpl )
}


#endif

