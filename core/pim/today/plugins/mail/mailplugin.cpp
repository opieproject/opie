/*
 * mailplugin.cpp
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


#include "mailplugin.h"
#include "mailpluginwidget.h"


MailPlugin::MailPlugin() {
}

MailPlugin::~MailPlugin() {
}

QString MailPlugin::pluginName() const {
    return QObject::tr( "Mail plugin" );
}

double MailPlugin::versionNumber() const {
    return 0.6;
}

QString MailPlugin::pixmapNameWidget() const {
    return "mail/desktopicon";
}

QWidget* MailPlugin::widget( QWidget * wid ) {
    return new MailPluginWidget( wid, "Mail" );
}

QString MailPlugin::pixmapNameConfig() const {
    return 0l;
}

TodayConfigWidget* MailPlugin::configWidget( QWidget* wid ) {
    return 0l;
}

QString MailPlugin::appName() const {
    return "mail";
}


bool MailPlugin::excludeFromRefresh() const {
    return false;
}

