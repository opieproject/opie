/*
 * datebookpluginimpl.h
 *
 * copyright   : (c) 2002, 2003, 2004 by Maximilian Reiﬂ
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

#ifndef DATEBOOK_PLUGIN_IMPL_H
#define DATEBOOK_PLUGIN_IMPL_H

#include <opie2/todayplugininterface.h>

class DatebookPlugin;

class DatebookPluginImpl : public TodayPluginInterface{

public:
    DatebookPluginImpl();
    virtual ~DatebookPluginImpl();

    QRESULT queryInterface( const QUuid &,  QUnknownInterface** );
    Q_REFCOUNT

    virtual TodayPluginObject *guiPart();

private:
    DatebookPlugin *datebookPlugin;
};

#endif
