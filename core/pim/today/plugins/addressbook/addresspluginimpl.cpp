/*
 * addresspluginimpl.cpp
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
#include "addresspluginimpl.h"

AddressBookPluginImpl::AddressBookPluginImpl() {
    addressbookPlugin  = new AddressBookPlugin();
}

AddressBookPluginImpl::~AddressBookPluginImpl() {
    delete addressbookPlugin;
}


TodayPluginObject* AddressBookPluginImpl::guiPart() {
    return addressbookPlugin;
}

QRESULT AddressBookPluginImpl::queryInterface( const QUuid & uuid,  QUnknownInterface **iface ) {
    *iface = 0;
    if ( ( uuid == IID_QUnknown ) || ( uuid == IID_TodayPluginInterface ) )  {
        *iface = this, (*iface)->addRef();
    }
    return QS_OK;

}

Q_EXPORT_INTERFACE() {
    Q_CREATE_INSTANCE( AddressBookPluginImpl );
}
