/***************************************************************************
  mwidget.cpp  -  description
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
#include "mwidget.h"
#include <qlayout.h>
#include <qhbox.h>
#include <qlabel.h>

MainWidget::MainWidget( QWidget* parent, const char* name )
	: QFrame( parent, name )
{
	BallPainter *bPainter = new BallPainter();

	QGridLayout *Form1Layout = new QGridLayout( this );
	Form1Layout->setSpacing( 4 );
	Form1Layout->setMargin( 4 );
	QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	Form1Layout->addItem( spacer, 0, 0 );

	lsb = new LinesBoard(bPainter, this);

	Form1Layout->addWidget( lsb, 0, 1 );
	QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	Form1Layout->addItem( spacer_2, 0, 2 );

	QHBox *bottom=new QHBox(this);
	
	mPoints=new QLabel(bottom);
	bottom->setStretchFactor(mPoints, 2);
	
	lPrompt = new LinesPrompt(bPainter, bottom);

	Form1Layout->addWidget( bottom, 1, 1 );
	QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
	Form1Layout->addItem( spacer_3, 2, 1 );


	connect(lsb, SIGNAL(endTurn()), parent, SLOT(makeTurn()));
	connect(lsb, SIGNAL(eraseLine(int)), parent, SLOT(addScore(int)));
	connect(lsb, SIGNAL(endGame()), parent, SLOT(stopGame()));

	connect(lPrompt, SIGNAL(PromptPressed()), parent, SLOT(switchPrompt()));

}

void MainWidget::setMessage(const QString &message)
{
	mPoints->setText(message);
}


/*
Destructor: deallocates memory for contents
 */

MainWidget::~MainWidget()
{
}

LinesBoard * MainWidget::GetLsb()
{
	return lsb;
}

LinesPrompt * MainWidget::GetPrompt()
{
	return lPrompt;
}
