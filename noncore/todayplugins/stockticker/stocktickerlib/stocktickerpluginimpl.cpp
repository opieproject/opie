/*
 * stocktickerpluginimpl.cpp
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



#include "stocktickerplugin.h"
#include "stocktickerpluginimpl.h"

StockTickerPluginImpl::StockTickerPluginImpl() {
    stocktickerPlugin  = new StockTickerPlugin();
}

StockTickerPluginImpl::~StockTickerPluginImpl() {
    delete stocktickerPlugin;
}


TodayPluginObject* StockTickerPluginImpl::guiPart() {
    return stocktickerPlugin;
}

QRESULT StockTickerPluginImpl::queryInterface( const QUuid & uuid,  QUnknownInterface **iface ) {
    *iface = 0;
    if ( ( uuid == IID_QUnknown ) || ( uuid == IID_TodayPluginInterface ) )  {
        *iface = this, (*iface)->addRef();
    }
    return QS_OK;

}

Q_EXPORT_INTERFACE() {
    Q_CREATE_INSTANCE( StockTickerPluginImpl );
}
