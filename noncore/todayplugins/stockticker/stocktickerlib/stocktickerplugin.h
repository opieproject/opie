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

#include <opie2/oclickablelabel.h>
#include <opie2/todayplugininterface.h>
#include <opie2/todayconfigwidget.h>

#include <qstring.h>
#include <qwidget.h>

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
