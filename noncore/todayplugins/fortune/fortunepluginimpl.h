/*
 * todopluginimpl.h
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

#ifndef FORTUNE_PLUGIN_IMPL_H
#define FORTUNE_PLUGIN_IMPL_H

#include <opie/todayplugininterface.h>

class FortunePlugin;

class FortunePluginImpl : public TodayPluginInterface
{

public:
	FortunePluginImpl();
	virtual ~FortunePluginImpl();

	QRESULT queryInterface( const QUuid &,  QUnknownInterface** );
	Q_REFCOUNT

	virtual TodayPluginObject *guiPart();

private:
	FortunePlugin *fortunePlugin;
	ulong ref;
};

#endif
