/****************************************************************************
* libtremorpluginimpl.cpp
*
* Copyright (C) 2002 Latchesar Ionkov <lucho@ionkov.net>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****************************************************************************/

#include "libtremorplugin.h"
#include "libtremorpluginimpl.h"


LibTremorPluginImpl::LibTremorPluginImpl()
    : libtremorplugin(0), ref(0)
{
}


LibTremorPluginImpl::~LibTremorPluginImpl()
{
    if ( libtremorplugin )
	delete libtremorplugin;
}


MediaPlayerDecoder *LibTremorPluginImpl::decoder()
{
    if ( !libtremorplugin )
	libtremorplugin = new LibTremorPlugin;
    return libtremorplugin;
}


MediaPlayerEncoder *LibTremorPluginImpl::encoder()
{
    return NULL;
}


#ifndef QT_NO_COMPONENT


QRESULT LibTremorPluginImpl::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
{
    *iface = 0;
    if ( ( uuid == IID_QUnknown ) || ( uuid == IID_MediaPlayerPlugin ) )
	*iface = this, (*iface)->addRef();
    else
	return QS_FALSE;
    return QS_OK;
}


Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( LibTremorPluginImpl )
}


#endif

