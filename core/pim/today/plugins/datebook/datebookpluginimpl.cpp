/*
 * datebookpluginimpl.cpp
 *
 * copyright   : (c) 2002 by Maximilian Reiß
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
#include "datebookpluginimpl.h"

DatebookPluginImpl::DatebookPluginImpl() {
    datebookPlugin  = new DatebookPlugin();
}

DatebookPluginImpl::~DatebookPluginImpl() {
}

TodayPluginObject* DatebookPluginImpl::guiPart() {
    return datebookPlugin;
}

QRESULT DatebookPluginImpl::queryInterface( const QUuid & uuid,  QUnknownInterface **iface ) {
    *iface = 0;
    if ( ( uuid == IID_QUnknown ) || ( uuid == IID_TodayPluginInterface ) )  {
        *iface = this, (*iface)->addRef();
    }
    return QS_OK;

}

Q_EXPORT_INTERFACE() {
    Q_CREATE_INSTANCE( DatebookPluginImpl );
}
