/*
 * exampleplugin.h
 *
 * email       : harlekin@handhelds.org
 *
 */


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
