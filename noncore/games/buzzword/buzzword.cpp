/*
 * Copyright (C) 2002 Martin Imobersteg <imm@gmx.ch> 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License,Life or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qlayout.h>
#include <qmessagebox.h>
#include <qmainwindow.h>
#include <qlabel.h>
#include <qgrid.h>
#include <qcolor.h>
#include <qbutton.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <qmessagebox.h>
#include <qdir.h>

#include <math.h>
#include <stdlib.h>

#include <list>
#include <string>

#include <qpe/qpeapplication.h>

#include "buzzword.h"

// sponsered by rikkus :)
bool random_compare(const QString &, const QString &)
{
  return (rand() % 2) > 0.5;
}

BuzzLabel::BuzzLabel( QWidget *parent, const char *name )
: QLabel( parent, name )
{
}

void BuzzLabel::mousePressEvent(QMouseEvent *e)
{
	if(e->button() == LeftButton)
	{
		emit clicked();
	}
}

BuzzItem::BuzzItem( int row, int column, QString text, QWidget *parent, const char *name )
: QVBox( parent, name ), _row(row), _column(column)
{
	setFrameStyle( QFrame::Panel | QFrame::Raised );
	setLineWidth( 1 );
	label = new BuzzLabel(this, "label");
	label->setText(text);
	label->setAlignment( int( QLabel::AlignCenter ) );

	connect( label, SIGNAL(clicked()), this, SLOT(flip()) );
}

void BuzzItem::flip()
{
	setLineWidth( 1 );
	label->setBackgroundColor(label->colorGroup().highlight());
	emit clicked(_row, _column);
}

BuzzWord::BuzzWord() : QMainWindow(0)
{
	setCaption(tr("buZzword"));

	menu = menuBar();
	game = new QPopupMenu;
	game->insertItem(tr("&New game"), this, SLOT(newGame()), Key_N );
	game->insertSeparator();
	game->insertItem(tr("&About"), this, SLOT(about()));
	menu->insertItem( tr("&Game"), game );

	gridVal = 4;
	grid = NULL;
	gameOver = false;
	newGame();
}

void BuzzWord::drawGrid()
{
	std::list<QString> l;

	QString path = QPEApplication::qpeDir()+"share/buzzword/";
	QFile f( path + "buzzwords" );
	if ( !f.open( IO_ReadOnly ) )
		return;

	QTextStream t( &f );

	while (!t.atEnd())
	{
		l.push_back(t.readLine());
	}

	f.close();

	l.sort(random_compare);
	
	grid = new QGrid(gridVal, this);
	grid->setFixedSize(240,240);

	for( int c = 0 ; c < gridVal ; c++ )
	{
		for( int r = 0 ; r < gridVal ; r++ )
		{
			QString word = QStringList::split(" ", l.front()).join("\n");
			BuzzItem* bi = new BuzzItem( c, r, word, grid );
			connect( bi, SIGNAL(clicked(int, int)), this, SLOT(clicked(int,int)) );
			map[c][r] = 0;

			l.pop_front();
		}
	}
}

void BuzzWord::clicked(int row, int column)
{
	if ( ! gameOver )
	{
		int rowTotal = 0;
		int columnTotal = 0;

		map[column][row] = 1;
		
		for( int c = 0 ; c < gridVal ; c++ )
		{
			for( int r = 0 ; r < gridVal ; r++ )
			{
				if ( map[c][r] == 1 )
					rowTotal++;

				if ( rowTotal == 4 )
				{
					bingo();
				}
			}
			rowTotal = 0;
		}

		for( int r = 0 ; r < gridVal ; r++ )
		{
			for( int c = 0 ; c < gridVal ; c++ )
			{
				if ( map[c][r] == 1 )
					columnTotal++;

				if ( columnTotal == 4 )
				{
					bingo();
				}
			}
			columnTotal = 0;
		}

		if ( map[0][0] && map[1][1] && map[2][2] && map[3][3] )
			bingo();

		if ( map[0][3] && map[1][2] && map[2][1] && map[3][0] )
            bingo();
	}
}

void BuzzWord::bingo()
{
	gameOver = true;
    QMessageBox::information( this, "BUZZWORD", tr("<h1><b>BINGO !</b></h1>"));
}

void BuzzWord::about()
{
   QMessageBox::information( this, "About", "buZzword 1.0\n(c) 2002 Martin Imobersteg\n\nThis program is distributed\nunder the terms of the GPL v2." );
}

void BuzzWord::newGame()
{
	gameOver = false;
	delete grid;
	drawGrid();
	setCentralWidget(grid);
}
