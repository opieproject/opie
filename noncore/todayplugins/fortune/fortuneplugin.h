/*
 * fortuneplugin.h
 *
 * copyright   : (c) 2002 by Chris Larson
 * email       : kergoth@handhelds.org
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

#ifndef FORTUNE_PLUGIN_H
#define FORTUNE_PLUGIN_H

#include <opie2/oclickablelabel.h>
#include <opie2/todayplugininterface.h>

#include <qstring.h>
#include <qwidget.h>

class FortunePlugin : public TodayPluginObject
{

public:
    FortunePlugin();
    ~FortunePlugin();

    QString pluginName()  const;
    double versionNumber() const;
    QString pixmapNameWidget() const;
    QWidget* widget( QWidget * );
    QString pixmapNameConfig() const;
    TodayConfigWidget* configWidget( QWidget * );
    QString appName() const;
    bool excludeFromRefresh() const;
};

#endif
