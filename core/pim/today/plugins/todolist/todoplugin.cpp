/*
 * todoplugin.cpp
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



#include "todoplugin.h"
#include "todopluginconfig.h"


TodolistPlugin::TodolistPlugin() {
}

TodolistPlugin::~TodolistPlugin() {
    delete (TodolistPluginWidget*)m_widget;
}

QString TodolistPlugin::pluginName() const {
    return QObject::tr( "Todolist plugin" );
}

double TodolistPlugin::versionNumber() const {
    return 0.9;
}

QString TodolistPlugin::pixmapNameWidget() const {
    return "todo/TodoList";
}

QWidget* TodolistPlugin::widget( QWidget *wid ) {
       if(!m_widget) {
        m_widget = new TodolistPluginWidget( wid, "Todolist" );
       }
    return m_widget;
}

QString TodolistPlugin::pixmapNameConfig() const {
    return "todo/TodoList";
}

TodayConfigWidget* TodolistPlugin::configWidget( QWidget* wid ) {
    return new TodolistPluginConfig( wid , "Todolist" );
}

QString TodolistPlugin::appName() const {
    return  "todolist";
}


bool TodolistPlugin::excludeFromRefresh() const {
    return false;
}

void TodolistPlugin::refresh()  {
    if ( m_widget )  {
        m_widget->refresh();
    }
}

void TodolistPlugin::reinitialize()  {
    if ( m_widget )  {
        m_widget->reinitialize();
    }
}
