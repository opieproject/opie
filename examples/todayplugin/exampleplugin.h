/*
 * exampleplugin.h
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


#ifndef EXAMPLE_PLUGIN_H
#define EXAMPLE_PLUGIN_H

#include <qwidget.h>
#include <qguardedptr.h>

#include <opie/todayplugininterface.h>
#include <opie/todayconfigwidget.h>

#include "examplepluginwidget.h"

// implementation of the today plugin interface
class ExamplePlugin : public TodayPluginObject {

public:
    ExamplePlugin();
    ~ExamplePlugin();

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
   QGuardedPtr<ExamplePluginWidget> m_widget;
};

#endif
