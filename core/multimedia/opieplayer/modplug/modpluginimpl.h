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
#ifndef MOD_PLUGIN_IMPL_H
#define MOD_PLUGIN_IMPL_H

#include <qpe/mediaplayerplugininterface.h>

class ModPlugin;

class ModPluginImpl : public MediaPlayerPluginInterface
{
public:
    ModPluginImpl();
    virtual ~ModPluginImpl();
#ifndef QT_NO_COMPONENT

    QRESULT queryInterface( const QUuid &uuid, QUnknownInterface **iface );
    Q_REFCOUNT

#endif

    virtual MediaPlayerDecoder *decoder();
    virtual MediaPlayerEncoder *encoder();

private:
    ModPlugin *m_plugin;
    ulong ref;
};

#endif
