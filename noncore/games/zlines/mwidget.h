/***************************************************************************
                          mwidget.h  -  description
                             -------------------
    begin                : Fri May 19 2000
    copyright            : (C) 2000 by Roman Merzlyakov
    email                : roman@sbrf.barrt.ru
    copyright            : (C) 2000 by Roman Razilov
    email                : Roman.Razilov@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MWIDGET_H
#define MWIDGET_H

#include <qwidget.h>
#include <qpixmap.h>
#include <qtimer.h>
#include <qframe.h>
#include "linesboard.h"
#include "prompt.h"

class QLabel;

class MainWidget : public QFrame
{
Q_OBJECT
	LinesBoard * lsb;
	LinesPrompt * lPrompt;
	QLabel *mPoints;
	
public:
	MainWidget( QWidget* parent=0, const char* name=0 );
	~MainWidget();
	LinesBoard * GetLsb();
	LinesPrompt * GetPrompt();

public slots:
	void setMessage(const QString &message);
};

#endif
