/*
 * fortunepluginwidget.h
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

#ifndef FORTUNE_PLUGIN_WIDGET_H
#define FORTUNE_PLUGIN_WIDGET_H

#include <qstring.h>
#include <qwidget.h>

#include <opie/oticker.h>
#include <opie/oprocess.h>


class FortunePluginWidget : public QWidget
{

	Q_OBJECT

public:
	FortunePluginWidget( QWidget *parent,  const char *name );
	~FortunePluginWidget();

private:
	OTicker *fortune;
	OProcess *fortuneProcess;

	void getFortune();

private slots:
	void slotStdOut( OProcess*, char*, int );
};

#endif
