/***************************************************************************
                          mainwin.cpp  -  description
                             -------------------
    begin                : Mon Aug 26 13:32:30 BST 2002
    copyright            : (C) 2002 by Andy Qua
    email                : andy.qua@blueyonder.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>

#include "mainwin.h"
#include "datamgr.h"
#include "networkpkgmgr.h"
#include "settingsimpl.h"
#include "helpwindow.h"
#include "utils.h"
#include "global.h"

MainWindow :: MainWindow( QWidget *p, char *name )
	:	QMainWindow( p, name )
{
#ifdef QWS
    showMaximized();
#endif

    setCaption( "AQPkg - Package Manager" );

    // Create our menu
	QPopupMenu *help = new QPopupMenu( this );

	help->insertItem( "&General", this, SLOT(displayHelp()), Qt::CTRL+Qt::Key_H );
	help->insertItem( "&About", this, SLOT(displayAbout()), Qt::CTRL+Qt::Key_A );

    QPopupMenu *settings = new QPopupMenu( this );
	settings->insertItem( "&Settings", this, SLOT(displaySettings()), Qt::CTRL+Qt::Key_S );

    QPopupMenu *edit = new QPopupMenu( this );
	edit->insertItem( "&Find", this, SLOT(searchForPackage()), Qt::CTRL+Qt::Key_F );
	edit->insertItem( "&Find Next", this, SLOT(repeatSearchForPackage()), Qt::CTRL+Qt::Key_R );

	// Create the main menu
	QMenuBar *menu = menuBar();  //new QMenuBar( this );
	menu->insertItem( "&Settings", settings );
	menu->insertItem( "&Edit", edit );
	menu->insertItem( "&Help", help );

    mgr = new DataManager();
    mgr->loadServers();

	stack = new QWidgetStack( this );

	networkPkgWindow = new NetworkPackageManager( mgr, stack );
	stack->addWidget( networkPkgWindow, 1 );

	setCentralWidget( stack );
	stack->raiseWidget( networkPkgWindow );
}

MainWindow :: ~MainWindow()
{
	delete networkPkgWindow;
}

void MainWindow :: setDocument( const QString &doc )
{
    // Remove path from package
    QString package = Utils::getPackageNameFromIpkFilename( doc );
    cout << "Selecting package " << package << endl;
    networkPkgWindow->selectLocalPackage( package );
}

void MainWindow :: displaySettings()
{
    SettingsImpl *dlg = new SettingsImpl( mgr, this, "Settings", true );
    if ( dlg->showDlg( 0 ) )
    	networkPkgWindow->updateData();
    delete dlg;
}

void MainWindow :: displayHelp()
{
    HelpWindow *dlg = new HelpWindow( this );
    dlg->exec();
    delete dlg;    
}

void MainWindow :: searchForPackage()
{
    networkPkgWindow->searchForPackage( false );
}

void MainWindow :: repeatSearchForPackage()
{
    networkPkgWindow->searchForPackage( true );
}

void MainWindow :: displayAbout()
{
    QMessageBox::about( this, "About AQPkg", VERSION_TEXT );
}
