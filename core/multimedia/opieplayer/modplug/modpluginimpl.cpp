/* This file is part of the KDE project
   Copyright (C) 2002 Simon Hausmann <hausmann@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "modplugin.h"
#include "modpluginimpl.h"

ModPluginImpl::ModPluginImpl()
    :m_plugin( 0 ), ref( 0 )
{
}

ModPluginImpl::~ModPluginImpl()
{
    delete m_plugin;
}

#ifndef QT_NO_COMPONENT

QRESULT ModPluginImpl::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
{
    *iface = 0;
    if ( ( uuid == IID_QUnknown ) || ( uuid == IID_MediaPlayerPlugin ) )
    *iface = this,  (*iface)->addRef();
    return QS_OK;
}

Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( ModPluginImpl )
}

#endif

MediaPlayerDecoder *ModPluginImpl::decoder()
{
    if ( !m_plugin )
        m_plugin = new ModPlugin;
    return m_plugin;
}

MediaPlayerEncoder *ModPluginImpl::encoder()
{
    return 0;
}
