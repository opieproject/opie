/*
 * addressplugin.cpp
 *
 * copyright   : (c) 2003 by Stefan Eilers
 * email       : eilers.stefan@epost.de
 *
 * This implementation was derived from the todolist plugin implementation
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



#include "addressplugin.h"
#include "addresspluginconfig.h"

AddressBookPlugin::AddressBookPlugin():
	m_abWidget( 0l )
{
}

AddressBookPlugin::~AddressBookPlugin() {
}

QString AddressBookPlugin::pluginName() const {
    return QObject::tr( "AddressBook plugin" );
}

double AddressBookPlugin::versionNumber() const {
    return 0.2;
}

QString AddressBookPlugin::pixmapNameWidget() const {
    return "AddressBook";
}

QWidget* AddressBookPlugin::widget( QWidget *wid ) {
    m_abWidget = new AddressBookPluginWidget( wid, "AddressBook" );
    return m_abWidget;
}

QString AddressBookPlugin::pixmapNameConfig() const {
    return "AddressBook";
}

TodayConfigWidget* AddressBookPlugin::configWidget( QWidget* wid ) {
    return new AddressBookPluginConfig( wid , "AddressBook" );
}

QString AddressBookPlugin::appName() const {
    return  "addressbook";
}


bool AddressBookPlugin::excludeFromRefresh() const {
    return false;
}

void AddressBookPlugin::refresh()
{
    m_abWidget->refresh( NULL );
}
