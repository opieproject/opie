/*
 * fortuneplugin.cpp
 *
 * copyright   : (c) 2002 by Chris Larson
 * email       : kergoth@handhelds.org
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



#include "fortuneplugin.h"
#include "fortunepluginwidget.h"


FortunePlugin::FortunePlugin()
{
}

FortunePlugin::~FortunePlugin()
{
}

QString FortunePlugin::pluginName() const
{
	return QObject::tr( "Fortune plugin" );
}

double FortunePlugin::versionNumber() const
{
	return 0.1;
}

QString FortunePlugin::pixmapNameWidget() const
{
	return "Fortune";
}

QWidget* FortunePlugin::widget( QWidget *wid )
{
	return new FortunePluginWidget( wid, "Fortune" );
}

QString FortunePlugin::pixmapNameConfig() const
{
	return "Fortune";
}

TodayConfigWidget* FortunePlugin::configWidget( QWidget* wid )
{
//    return new FortunePluginConfig( wid , "Fortune" );
	return NULL;
}

QString FortunePlugin::appName() const
{
	return  "fortune";
}


bool FortunePlugin::excludeFromRefresh() const
{
	return false;
}
