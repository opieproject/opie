/*
 * todoplugin.h
 *
 * copyright   : (c) 2002,2003, 2004 by Maximilian Reiﬂ
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


#include "todopluginwidget.h"

#include <opie2/oclickablelabel.h>
#include <opie2/todayplugininterface.h>

#include <qstring.h>
#include <qguardedptr.h>
#include <qwidget.h>


class TodolistPlugin : public TodayPluginObject {

public:
    TodolistPlugin();
    ~TodolistPlugin();

    QString pluginName()  const;
    double versionNumber() const;
    QString pixmapNameWidget() const;
    QWidget* widget(QWidget *);
    QString pixmapNameConfig() const;
    TodayConfigWidget* configWidget(QWidget *);
    QString appName() const;
    bool excludeFromRefresh() const;
    void refresh();
    void reinitialize();

 private:
    QGuardedPtr<TodolistPluginWidget> m_widget;
};

#endif
