/*
 * stocktickerplugin.h
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


#ifndef STOCKTICKER_PLUGIN_H
#define STOCKTICKER_PLUGIN_H

#include <qstring.h>
#include <qwidget.h>

#include <opie/tododb.h>
#include <opie/oclickablelabel.h>

#include <opie/todayplugininterface.h>
#include <opie/todayconfigwidget.h>

class StockTickerPlugin : public TodayPluginObject {

public:
    StockTickerPlugin();
    ~StockTickerPlugin();

    QString pluginName()  const;
    double versionNumber() const;
    QString pixmapNameWidget() const;
    QWidget* widget(QWidget *);
    QString pixmapNameConfig() const;
    TodayConfigWidget* configWidget(QWidget *);
    QString appName() const;
    bool excludeFromRefresh() const;

};

#endif
