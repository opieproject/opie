/*
 * exampleplugin.cpp
 *
 * email       : harlekin@handhelds.org
 *
 */
#include "exampleplugin.h"

ExamplePlugin::ExamplePlugin() {
    m_widget = 0l;
}

ExamplePlugin::~ExamplePlugin() {
    delete (ExamplePluginWidget*)m_widget;
}

QString ExamplePlugin::pluginName() const {
    return QObject::tr( "Example plugin" );
}

double ExamplePlugin::versionNumber() const {
    return 0.1;
}

// this sets the image that will be shown on the left side of the plugin
QString ExamplePlugin::pixmapNameWidget() const {
    return QString::fromLatin1("Tux");
}

QWidget* ExamplePlugin::widget( QWidget * wid ) {
    if(!m_widget) {
        m_widget = new ExamplePluginWidget( wid,  "Example" );
    }
    return m_widget;
}


// that would be the icon of the config widget in todays config view
QString ExamplePlugin::pixmapNameConfig() const {
    return 0l;
}

// No config widget yet, look at the datebook plugin for an example of that
TodayConfigWidget* ExamplePlugin::configWidget( QWidget* /*parent*/ ) {
    return 0l;
}

// add the binary name of the app to launch here
QString ExamplePlugin::appName() const {
    return QString::null;
}

// if the plugin should be excluded form the refresh cycles that can be set in the today app
bool ExamplePlugin::excludeFromRefresh() const {
    return false;
}

void ExamplePlugin::refresh()  {
    if ( m_widget )  {
        m_widget->refresh();
    }
}

void ExamplePlugin::reinitialize()  {
}
