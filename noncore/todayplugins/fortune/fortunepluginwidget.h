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

#include <opie2/oprocess.h>
#include <opie2/oticker.h>

#include <qstring.h>
#include <qwidget.h>

class FortunePluginWidget : public QWidget
{

	Q_OBJECT

public:
	FortunePluginWidget( QWidget *parent,  const char *name );
	~FortunePluginWidget();

private:
	Opie::Ui::OTicker *fortune;
	Opie::Core::OProcess *fortuneProcess;

	void getFortune();

private slots:
	void slotStdOut( Opie::Core::OProcess*, char*, int );
};

#endif
