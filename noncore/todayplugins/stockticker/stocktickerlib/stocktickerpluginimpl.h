/*
 * stocktickerpluginimpl.h
 *
 * copyright   : (c) 2002 by L.J. Potter
 * email       : llornkcor@handhelds.org
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

#ifndef STOCKTICKER_PLUGIN_IMPL_H
#define STOCKTICKER_PLUGIN_IMPL_H

#include <opie/todayplugininterface.h>

class StockTickerPlugin;

class StockTickerPluginImpl : public TodayPluginInterface{

public:
    StockTickerPluginImpl();
    virtual ~StockTickerPluginImpl();

    QRESULT queryInterface( const QUuid &,  QUnknownInterface** );
    Q_REFCOUNT

    virtual TodayPluginObject *guiPart();

private:
    StockTickerPlugin *stocktickerPlugin;
    ulong ref;
};

#endif
