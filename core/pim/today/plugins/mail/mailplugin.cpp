/*
 * mailplugin.cpp
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


#include "mailplugin.h"

MailPlugin::MailPlugin() {
    m_widget = 0l;
}

MailPlugin::~MailPlugin() {
    delete (MailPluginWidget*)m_widget;
}

QString MailPlugin::pluginName() const {
    return QObject::tr( "Mail plugin" );
}

double MailPlugin::versionNumber() const {
    return 0.6;
}

QString MailPlugin::pixmapNameWidget() const {
    return "opiemail/desktopicon";
}

QWidget* MailPlugin::widget( QWidget * wid ) {
    if(!m_widget) {
        m_widget = new MailPluginWidget( wid,  "Datebook" );
    }
    return m_widget;
}


QString MailPlugin::pixmapNameConfig() const {
    return 0l;
}

TodayConfigWidget* MailPlugin::configWidget( QWidget* ) {
    return 0l;
}

QString MailPlugin::appName() const {
    return "opiemail";
}


bool MailPlugin::excludeFromRefresh() const {
    return false;
}

void MailPlugin::refresh()  {
    if ( m_widget )  {
        m_widget->refresh();
    }
}

void MailPlugin::reinitialize()  {
}
