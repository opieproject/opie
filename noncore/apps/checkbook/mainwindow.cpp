/*
                             This file is part of the OPIE Project
               =.
             .=l.            Copyright (c)  2002 Dan Williams <drw@handhelds.org>
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
#include "password.h"
#include "checkbook.h"

#include <qpe/config.h>
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
#include <qwhatsthis.h>

MainWindow::MainWindow()
	: QMainWindow( 0x0, 0x0, WStyle_ContextHelp )
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
	QAction *a = new QAction( tr( "New" ), Resource::loadPixmap( "new" ), QString::null, 0, this, 0 );
	a->setWhatsThis( tr( "Click here to create a new checkbook.\n\nYou also can select New from the Checkbook menu." ) );
	connect( a, SIGNAL( activated() ), this, SLOT( slotNew() ) );
	a->addTo( popup );
	a->addTo( bar );

	actionOpen = new QAction( tr( "Edit" ), Resource::loadPixmap( "edit" ), QString::null,
							  0, this, 0 );
	actionOpen->setWhatsThis( tr( "Select a checkbook and then click here to edit it.\n\nYou also can select Edit from the Checkbook menu, or click and hold on a checkbook name." ) );
	connect( actionOpen, SIGNAL( activated() ), this, SLOT( slotEdit() ) );
	actionOpen->addTo( popup );
	actionOpen->addTo( bar );

	actionDelete = new QAction( tr( "Delete" ), Resource::loadPixmap( "trash" ), QString::null,
								0, this, 0 );
	actionDelete->setWhatsThis( tr( "Select a checkbook and then click here delete it.\n\nYou also can select Delete from the Checkbook menu." ) );
	connect( actionDelete, SIGNAL( activated() ), this, SLOT( slotDelete() ) );
	actionDelete->addTo( popup );
	actionDelete->addTo( bar );

	mb->insertItem( tr( "Checkbook" ), popup );

	// Build Checkbook selection list control
	cbList = new QListBox( this );
	QWhatsThis::add( cbList, tr( "This is a listing of all checkbooks currently available." ) );
	QPEApplication::setStylusOperation( cbList->viewport(), QPEApplication::RightOnHold );
	connect( cbList, SIGNAL( rightButtonPressed( QListBoxItem *, const QPoint & ) ),
			 this, SLOT( slotEdit() ) );
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

	currencySymbol = "$";
}

MainWindow::~MainWindow()
{
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

void MainWindow::slotEdit()
{
	QString currname = cbList->currentText();
	
	QString tempstr = cbDir;
	tempstr.append( currname );
	tempstr.append( ".qcb" );
	
	Config config( tempstr, Config::File );
	config.setGroup( "Account" );
	QString password = config.readEntryCrypt( "Password", "" );
	if ( password != "" )
	{
		Password *pw = new Password( this, tr( "Enter password" ), tr( "Please enter your password:" ) );
		if ( pw->exec() != QDialog::Accepted || pw->password != password  )
		{
			delete pw;
			return;
		}
		delete pw;
	}

	Checkbook *currcb = new Checkbook( this, currname, cbDir, currencySymbol );
	currcb->showMaximized();
	if ( currcb->exec() == QDialog::Accepted )
	{
		QString newname = currcb->getName();
		if ( currname != newname )
		{
			cbList->changeItem( newname, cbList->currentItem() );
			cbList->sort();

			QFile f( tempstr );
			if ( f.exists() )
			{
				f.remove();
			}
		}
		delete currcb;
	}
}

void MainWindow::slotDelete()
{
	if ( QPEMessageBox::confirmDelete ( this, tr( "Delete checkbook" ), cbList->currentText() ) )
	{
		QString tempstr = cbDir;
		tempstr.append( cbList->currentText() );
		tempstr.append( ".qcb" );
		QFile f( tempstr );
		if ( f.exists() )
		{
			f.remove();
		}

		cbList->removeItem( cbList->currentItem() );
	}
}
