/*
                             This file is part of the OPIE Project
               =.
             .=l.            Copyright (c)  2002 Dan Williams <williamsdr@acm.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "mainwindow.h"
#include "checkbook.h"

#include <qpe/global.h>
#include <qpe/qpeapplication.h>
#include <qpe/qpemenubar.h>
#include <qpe/qpemessagebox.h>
#include <qpe/qpetoolbar.h>
#include <qpe/resource.h>

#include <qaction.h>
#include <qdir.h>
#include <qlistbox.h>
#include <qpopupmenu.h>
#include <qstring.h>

MainWindow::MainWindow()
	: QMainWindow()
{
	setCaption( tr( "Checkbook" ) );

	cbDir = Global::applicationFileName( "checkbook", "" );

	// Build menu and tool bars
	setToolBarsMovable( FALSE );

	QPEToolBar *bar = new QPEToolBar( this );
	bar->setHorizontalStretchable( TRUE );
	QPEMenuBar *mb = new QPEMenuBar( bar );
	mb->setMargin( 0 );
	QPopupMenu *popup = new QPopupMenu( this );

	bar = new QPEToolBar( this );
	actionOpen = new QAction( tr( "Open" ), Resource::loadPixmap( "fileopen" ), QString::null,
							  0, this, 0 );
	connect( actionOpen, SIGNAL( activated() ), this, SLOT( slotOpen() ) );
	actionOpen->addTo( popup );
	actionOpen->addTo( bar );

	QAction *a = new QAction( tr( "New" ), Resource::loadPixmap( "new" ), QString::null, 0, this, 0 );
	connect( a, SIGNAL( activated() ), this, SLOT( slotNew() ) );
	a->addTo( popup );
	a->addTo( bar );

	actionDelete = new QAction( tr( "Delete" ), Resource::loadPixmap( "editdelete" ), QString::null,
								0, this, 0 );
	connect( actionDelete, SIGNAL( activated() ), this, SLOT( slotDelete() ) );
	actionDelete->addTo( popup );
	actionDelete->addTo( bar );

	popup->insertSeparator();

	a = new QAction( tr( "Exit" ), QString::null, 0, this, 0 );
	connect( a, SIGNAL( activated() ), this, SLOT( close() ) );
	a->addTo( popup );

	mb->insertItem( tr( "Checkbook" ), popup );

	// Build Checkbook selection list control
	cbList = new QListBox( this );
	QPEApplication::setStylusOperation( cbList->viewport(), QPEApplication::RightOnHold );
	connect( cbList, SIGNAL( rightButtonPressed( QListBoxItem *, const QPoint & ) ),
			 this, SLOT( slotOpen() ) );
	setCentralWidget( cbList );

	// Load Checkbook selection list
	QDir checkdir( cbDir );
	if (checkdir.exists() == true)
	{
		QStringList checkbooks = checkdir.entryList( "*.qcb", QDir::Files|QDir::Readable|QDir::Writable,
													 QDir::Time );
		for ( QStringList::Iterator it = checkbooks.begin(); it != checkbooks.end(); it++ )
		{
			(*it) = (*it).remove( (*it).find('.'), (*it).length() );
		}
		cbList->insertStringList( checkbooks );
	}
	cbList->sort();
	cbList->setSelected( 0, TRUE );

	currencySymbol = '$';
}

MainWindow::~MainWindow()
{
}

void MainWindow::slotOpen()
{
	QString currname = cbList->currentText();
	Checkbook *currcb = new Checkbook( this, currname, cbDir, currencySymbol );
	currcb->showMaximized();
	if ( currcb->exec() == QDialog::Accepted )
	{
		QString newname = currcb->getName();
		if ( currname != newname )
		{
			cbList->changeItem( newname, cbList->currentItem() );
			cbList->sort();

			QFile f( cbDir + currname + ".qcb" );
			if ( f.exists() )
			{
				f.remove();
			}
		}
		delete currcb;
	}
}

void MainWindow::slotNew()
{
	Checkbook *currcb = new Checkbook( this, "", cbDir, currencySymbol );
	currcb->showMaximized();
	if ( currcb->exec() == QDialog::Accepted )
	{
		cbList->insertItem( currcb->getName() );
		cbList->sort();
		delete currcb;
	}
}

void MainWindow::slotDelete()
{
	if ( QPEMessageBox::confirmDelete ( this, tr( "Delete checkbook" ), cbList->currentText() ) )
	{
		cbList->removeItem( cbList->currentItem() );

		QString name = cbDir + cbList->currentText() + ".qcb";
		QFile f( name );
		if ( f.exists() )
		{
			f.remove();
		}
	}
}
