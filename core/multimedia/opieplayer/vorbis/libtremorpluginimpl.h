/****************************************************************************
* libtremorpluginimpl.h
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

#ifndef LIBTREMOR_PLUGIN_IMPL_H 
#define LIBTREMOR_PLUGIN_IMPL_H


#include <qpe/mediaplayerplugininterface.h>


class LibTremorPlugin;


class LibTremorPluginImpl : public MediaPlayerPluginInterface
{
public:
    LibTremorPluginImpl();
    virtual ~LibTremorPluginImpl();

#ifndef QT_NO_COMPONENT

    QRESULT queryInterface( const QUuid&, QUnknownInterface** );
    Q_REFCOUNT

#endif

    virtual MediaPlayerDecoder *decoder();
    virtual MediaPlayerEncoder *encoder();

private:
    LibTremorPlugin *libtremorplugin;
    ulong ref;
};


#endif

