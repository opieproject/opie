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
#include "cbinfo.h"
#include "configuration.h"
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
#include <qcheckbox.h>
#include <qdir.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qpopupmenu.h>
#include <qstring.h>
#include <qwhatsthis.h>

MainWindow::MainWindow()
	: QMainWindow( 0x0, 0x0, WStyle_ContextHelp )
{
	setCaption( tr( "Checkbook" ) );

	cbDir = Global::applicationFileName( "checkbook", "" );
	lockIcon = Resource::loadPixmap( "locked" );

	// Load configuration options
	Config config( "checkbook" );
	config.setGroup( "Config" );
	currencySymbol = config.readEntry( "CurrencySymbol", "$" );
	showLocks = config.readBoolEntry( "ShowLocks", FALSE );
	showBalances = config.readBoolEntry( "ShowBalances", FALSE );

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

	popup->insertSeparator();

	a = new QAction( tr( "Configure" ), Resource::loadPixmap( "checkbook/config" ), QString::null, 0, this, 0 );
	a->setWhatsThis( tr( "Click here to configure this app." ) );
	connect( a, SIGNAL( activated() ), this, SLOT( slotConfigure() ) );
	a->addTo( popup );
	a->addTo( bar );

	mb->insertItem( tr( "Checkbook" ), popup );

	// Load Checkbook selection list
	checkbooks = new CBInfoList();

	QDir checkdir( cbDir );
	if (checkdir.exists() == true)
	{
		QStringList cblist = checkdir.entryList( "*.qcb", QDir::Files|QDir::Readable|QDir::Writable,
													 QDir::Time );
		CBInfo *cb = 0x0;
		QString filename;

		for ( QStringList::Iterator it = cblist.begin(); it != cblist.end(); it++ )
		{
			filename = cbDir;
			filename.append( (*it) );

			cb = new CBInfo( (*it).remove( (*it).find('.'), (*it).length() ), filename );
			checkbooks->inSort( cb );
		}
	}

	// Build Checkbook selection list control
	cbList = 0x0;
	buildList();
}

MainWindow::~MainWindow()
{
//	config.write();
}

void MainWindow::buildList()
{
	if ( cbList )
	{
		delete cbList;
	}

	cbList = new QListView( this );
	QWhatsThis::add( cbList, tr( "This is a listing of all checkbooks currently available." ) );

	if ( showLocks )
	{
		cbList->addColumn( Resource::loadIconSet( "locked" ), "", 24 );
		posName = 1;
	}
	else
	{
		posName = 0;
	}
	cbList->addColumn( tr( "Checkbook Name" ) );
	if ( showBalances )
	{
		int colnum = cbList->addColumn( tr( "Balance" ) );
		cbList->setColumnAlignment( colnum, Qt::AlignRight );
	}
	cbList->setAllColumnsShowFocus( TRUE );
	cbList->setSorting( posName );
	QPEApplication::setStylusOperation( cbList->viewport(), QPEApplication::RightOnHold );
	connect( cbList, SIGNAL( rightButtonPressed( QListViewItem *, const QPoint &, int ) ),
			 this, SLOT( slotEdit() ) );
	setCentralWidget( cbList );

	for ( CBInfo *cb = checkbooks->first(); cb; cb = checkbooks->next() )
	{
		addCheckbook( cb );
	}
}

void MainWindow::addCheckbook( CBInfo *cb )
{
	QListViewItem *lvi = new QListViewItem( cbList );
	if ( showLocks && !cb->password().isNull() )
	{
		lvi->setPixmap( 0, lockIcon );
	}
	lvi->setText( posName, cb->name() );
	if ( showBalances )
	{
		QString balance;
		balance.sprintf( "%s%.2f", currencySymbol.latin1(), cb->balance() );
		lvi->setText( posName + 1, balance );
	}
}

void MainWindow::buildFilename( const QString &name )
{
	tempFilename = cbDir;
	tempFilename.append( name );
	tempFilename.append( ".qcb" );
}

void MainWindow::slotNew()
{
	CBInfo *cb = new CBInfo();

	Checkbook *currcb = new Checkbook( this, cb, currencySymbol );
	currcb->showMaximized();
	if ( currcb->exec() == QDialog::Accepted )
	{
		// Save new checkbook
		buildFilename( cb->name() );
		cb->setFilename( tempFilename );
		cb->write();

		// Add to listbox
		checkbooks->inSort( cb );
		addCheckbook( cb );
	}
	delete currcb;
}

void MainWindow::slotEdit()
{

	QListViewItem *curritem = cbList->currentItem();
	if ( !curritem )
	{
		return;
	}
	QString currname = curritem->text( posName );

	CBInfo *cb = checkbooks->first();
	while ( cb )
	{
		if ( cb->name() == currname )
			break;
		cb = checkbooks->next();
	}
	if ( !cb )
	{
		return;
	}

	buildFilename( currname );
	float currbalance = cb->balance();
	bool currlock = !cb->password().isNull();

	if ( currlock )
	{
		Password *pw = new Password( this, tr( "Enter password" ), tr( "Please enter your password:" ) );
		if ( pw->exec() != QDialog::Accepted || pw->password != cb->password()  )
		{
			delete pw;
			return;
		}
		delete pw;
	}

	Checkbook *currcb = new Checkbook( this, cb, currencySymbol );
	currcb->showMaximized();
	if ( currcb->exec() == QDialog::Accepted )
	{
		QString newname = cb->name();
		if ( currname != newname )
		{
			// Update name if changed
			curritem->setText( posName, newname );
			cbList->sort();

			// Remove old file
			QFile f( tempFilename );
			if ( f.exists() )
			{
				f.remove();
			}

			// Get new filename
			buildFilename( newname );
			cb->setFilename( tempFilename );
		}

		cb->write();

		// Update lock if changed
		if ( showLocks && !cb->password().isNull() != currlock )
		{
			if ( !cb->password().isNull() )
				curritem->setPixmap( 0, lockIcon );
			else
				curritem->setPixmap( 0, nullIcon );
		}

		// Update balance if changed
		if ( showBalances && cb->balance() != currbalance )
		{
			QString tempstr;
			tempstr.sprintf( "%s%.2f", currencySymbol.latin1(), cb->balance() );
			curritem->setText( posName + 1, tempstr );
		}
	}
	delete currcb;
}

void MainWindow::slotDelete()
{
	QString currname = cbList->currentItem()->text( posName );

	if ( QPEMessageBox::confirmDelete ( this, tr( "Delete checkbook" ), currname ) )
	{
		buildFilename( currname );
		QFile f( tempFilename );
		if ( f.exists() )
		{
			f.remove();
		}

		delete cbList->currentItem();
	}
}

void MainWindow::slotConfigure()
{
	Configuration *cfgdlg = new Configuration( this, currencySymbol, showLocks, showBalances );
	cfgdlg->showMaximized();
	if ( cfgdlg->exec() == QDialog::Accepted )
	{
		currencySymbol = cfgdlg->symbolEdit->text();
		showLocks = cfgdlg->lockCB->isChecked();
		showBalances = cfgdlg->balCB->isChecked();

		Config config( "checkbook" );
		config.setGroup( "Config" );
		config.writeEntry( "CurrencySymbol", currencySymbol );
		config.writeEntry( "ShowLocks", showLocks );
		config.writeEntry( "ShowBalances", showBalances );
		config.write();

		buildList();
	}
	delete cfgdlg;
}
