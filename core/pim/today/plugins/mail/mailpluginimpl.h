/*
 * mailpluginimpl.h
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

#ifndef MAIL_PLUGIN_IMPL_H
#define MAIL_PLUGIN_IMPL_H

#include <opie/todayplugininterface.h>

class MailPlugin;

class MailPluginImpl : public TodayPluginInterface{

public:
    MailPluginImpl();
    virtual ~MailPluginImpl();

    QRESULT queryInterface( const QUuid &,  QUnknownInterface** );
    Q_REFCOUNT

    virtual TodayPluginObject *guiPart();

private:
    MailPlugin *mailPlugin;
    ulong ref;
};

#endif
