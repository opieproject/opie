
/*
 * datebookplugin.cpp
 *
 * copyright   : (c) 2002,2003 by Maximilian Reiß
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



#include "datebookplugin.h"
#include "datebookpluginconfig.h"


DatebookPlugin::DatebookPlugin()  {
}

DatebookPlugin::~DatebookPlugin() {
    delete (DatebookPluginWidget*)m_widget;
}

QString DatebookPlugin::pluginName() const {
    return QObject::tr( "Datebook plugin");
}

double DatebookPlugin::versionNumber() const {
    return 1.0;
}

QString DatebookPlugin::pixmapNameWidget() const {
    return "datebook/DateBook";
}

QWidget* DatebookPlugin::widget( QWidget* wid ) {
    if(!m_widget) {
        m_widget = new DatebookPluginWidget( wid,  "Datebook" );
    }
    return m_widget;
}

QString DatebookPlugin::pixmapNameConfig() const {
    return "datebook/DateBook";
}

TodayConfigWidget* DatebookPlugin::configWidget( QWidget* wid ) {
    return new DatebookPluginConfig( wid , "Datebook" );
}

QString DatebookPlugin::appName()  const {
    return "datebook";
}

bool DatebookPlugin::excludeFromRefresh() const {
    return false;
}

void DatebookPlugin::refresh() {
    if ( m_widget )  {
        m_widget->refresh();
    }
}

void DatebookPlugin::reinitialize()  {
    if ( m_widget )  {
        m_widget->reinitialize();
    }
}

