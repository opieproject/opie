/*
 * todopluginimpl.h
 *
 * copyright   : (c) 2002 by Maximilian Reiﬂ
 * email       : harlekin@handhelds.org
 *
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TODOLIST_PLUGIN_IMPL_H
#define TODOLIST_PLUGIN_IMPL_H

#include "../../todayplugininterface.h"

class TodolistPlugin;

class TodolistPluginImpl : public TodayPluginInterface{

public:
    TodolistPluginImpl();
    virtual ~TodolistPluginImpl();

    QRESULT queryInterface( const QUuid &,  QUnknownInterface** );
    Q_REFCOUNT

    virtual TodayPluginObject *guiPart();

private:
    TodolistPlugin *todolistPlugin;
    ulong ref;
};

#endif
