/*
 * todoplugin.h
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

#ifndef TODOLIST_PLUGIN_H
#define TODOLIST_PLUGIN_H

#include <qstring.h>
#include <qwidget.h>

#include <opie/tododb.h>
#include <opie/oclickablelabel.h>

#include "../../todayplugininterface.h"
#include "../../configwidget.h"

class TodolistPlugin : public TodayPluginObject {


public:
    TodolistPlugin();
    ~TodolistPlugin();

    QString pluginName()  const;
    double versionNumber() const;
    QString pixmapNameWidget() const;
    QWidget* widget(QWidget *);
    QString pixmapNameConfig() const;
    ConfigWidget* configWidget(QWidget *);
    QString appName() const;
    virtual int minHeight()  const;
    virtual int maxHeight()  const;
};

#endif
