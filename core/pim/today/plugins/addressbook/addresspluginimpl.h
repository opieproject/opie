/*
 * addresspluginimpl.h
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

#ifndef ADDRESSBOOK_PLUGIN_IMPL_H
#define ADDRESSBOOK_PLUGIN_IMPL_H

#include <opie/todayplugininterface.h>

class AddressBookPlugin;

class AddressBookPluginImpl : public TodayPluginInterface{

public:
    AddressBookPluginImpl();
    virtual ~AddressBookPluginImpl();

    QRESULT queryInterface( const QUuid &,  QUnknownInterface** );
    Q_REFCOUNT

    virtual TodayPluginObject *guiPart();

private:
    AddressBookPlugin *addressbookPlugin;
    ulong ref;
};

#endif
