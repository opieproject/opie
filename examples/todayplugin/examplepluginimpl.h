/*
 * examplepluginimpl.h
 *
 * copyright   : (c) 2004 by Maximilian Reiﬂ
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

#ifndef EXAMPLE_PLUGIN_IMPL_H
#define EXAMPLE_PLUGIN_IMPL_H

#include <opie/todayplugininterface.h>

class ExamplePlugin;

class ExamplePluginImpl : public TodayPluginInterface{

public:
    ExamplePluginImpl();
    virtual ~ExamplePluginImpl();

    QRESULT queryInterface( const QUuid &,  QUnknownInterface** );
    Q_REFCOUNT

    virtual TodayPluginObject *guiPart();

private:
    ExamplePlugin *examplePlugin;
    ulong ref;
};

#endif
