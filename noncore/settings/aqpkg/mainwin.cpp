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

#include <iostream>
using namespace std;

#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qtimer.h>

#include "mainwin.h"
#include "progresswidget.h"
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

    setCaption( tr( "AQPkg - Package Manager" ) );

    // Create our menu
    help = new QPopupMenu( this );
    help->insertItem( tr( "General" ), this, SLOT(displayHelp()) );
	help->insertItem( tr( "About" ), this, SLOT(displayAbout()) );

    settings = new QPopupMenu( this );
	settings->insertItem( tr( "Settings" ), this, SLOT(displaySettings()) );

    edit = new QPopupMenu( this );
	edit->insertItem( tr( "Find" ), this, SLOT(searchForPackage()) );
	edit->insertItem( tr( "Find Next" ), this, SLOT(repeatSearchForPackage()) );

    filter = new QPopupMenu( this );
	mnuShowUninstalledPkgsId = filter->insertItem( tr( "Show Non-Installed Packages" ), this, SLOT(filterUninstalledPackages()) );
	mnuShowInstalledPkgsId = filter->insertItem( tr( "Show Installed Packages" ), this, SLOT(filterInstalledPackages()) );
	mnuShowUpgradedPkgsId = filter->insertItem( tr( "Show Updated Packages" ), this, SLOT(filterUpgradedPackages()) );
    filter->insertSeparator();
    mnuFilterByCategory = filter->insertItem( tr( "Filter By Category" ), this, SLOT(filterCategory()) );
    mnuSetFilterCategory = filter->insertItem( tr( "Set Filter Category" ), this, SLOT(setFilterCategory()) );

	// Create the main menu
	menu = menuBar();  //new QMenuBar( this );
	menu->insertItem( tr( "Settings" ), settings );
	menu->insertItem( tr( "Edit" ), edit );
	menu->insertItem( tr( "Filter" ), filter );
	menu->insertItem( tr( "Help" ), help );

	// Create UI widgets
	stack = new QWidgetStack( this );

	progressWindow = new ProgressWidget( stack );
	stack->addWidget( progressWindow, 2 );

	networkPkgWindow = new NetworkPackageManager( stack );
	connect( networkPkgWindow, SIGNAL( appRaiseMainWidget() ), this, SLOT( raiseMainWidget() ) );
	connect( networkPkgWindow, SIGNAL( appRaiseProgressWidget() ), this, SLOT( raiseProgressWidget() ) );
	connect( networkPkgWindow, SIGNAL( progressSetSteps( int ) ), progressWindow, SLOT( setSteps( int ) ) );
	connect( networkPkgWindow, SIGNAL( progressSetMessage( const QString & ) ),
             progressWindow, SLOT( setMessage( const QString & ) ) );
	connect( networkPkgWindow, SIGNAL( progressUpdate( int ) ), progressWindow, SLOT( update( int ) ) );
	stack->addWidget( networkPkgWindow, 1 );

	setCentralWidget( stack );
	stack->raiseWidget( progressWindow );

	// Delayed call to finish initialization
	QTimer::singleShot( 100, this, SLOT( init() ) );

}

MainWindow :: ~MainWindow()
{
	delete mgr;
}

void MainWindow :: init()
{
    stack->raiseWidget( progressWindow );
    mgr = new DataManager();
	connect( mgr, SIGNAL( progressSetSteps( int ) ), progressWindow, SLOT( setSteps( int ) ) );
	connect( mgr, SIGNAL( progressSetMessage( const QString & ) ),
             progressWindow, SLOT( setMessage( const QString & ) ) );
	connect( mgr, SIGNAL( progressUpdate( int ) ), progressWindow, SLOT( update( int ) ) );
    mgr->loadServers();
    networkPkgWindow->setDataManager( mgr );
    networkPkgWindow->updateData();
    stack->raiseWidget( networkPkgWindow );
}

void MainWindow :: setDocument( const QString &doc )
{
    // Remove path from package
    QString package = Utils::getPackageNameFromIpkFilename( doc );
    std::cout << "Selecting package " << package << std::endl;
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
    QMessageBox::about( this, tr( "About AQPkg" ), tr( VERSION_TEXT ) );
}


void MainWindow :: filterUninstalledPackages()
{
    bool val;
    if ( filter->isItemChecked( mnuShowUninstalledPkgsId ) )
    {
        val = false;
        filter->setItemChecked( mnuShowUninstalledPkgsId, false );
    }
    else
    {
        val = true;
        filter->setItemChecked( mnuShowUninstalledPkgsId, true );
    }

    filter->setItemChecked( mnuShowInstalledPkgsId, false );
    networkPkgWindow->showOnlyInstalledPackages( false );
    filter->setItemChecked( mnuShowUpgradedPkgsId, false );
    networkPkgWindow->showUpgradedPackages( false );

    networkPkgWindow->showOnlyUninstalledPackages( val );

}

void MainWindow :: filterInstalledPackages()
{
    bool val;
    if ( filter->isItemChecked( mnuShowInstalledPkgsId ) )
    {
        val = false;
        filter->setItemChecked( mnuShowInstalledPkgsId, false );
    }
    else
    {
        val = true;
        filter->setItemChecked( mnuShowInstalledPkgsId, true );
    }

    filter->setItemChecked( mnuShowUninstalledPkgsId, false );
    networkPkgWindow->showOnlyUninstalledPackages( false );
    filter->setItemChecked( mnuShowUpgradedPkgsId, false );
    networkPkgWindow->showUpgradedPackages( false );

    networkPkgWindow->showOnlyInstalledPackages( val );
}

void MainWindow :: filterUpgradedPackages()
{
    bool val;
    if ( filter->isItemChecked( mnuShowUpgradedPkgsId ) )
    {
        val = false;
        filter->setItemChecked( mnuShowUpgradedPkgsId, false );
    }
    else
    {
        val = true;
        filter->setItemChecked( mnuShowUpgradedPkgsId, true );
    }

    filter->setItemChecked( mnuShowUninstalledPkgsId, false );
    networkPkgWindow->showOnlyUninstalledPackages( false );
    filter->setItemChecked( mnuShowInstalledPkgsId, false );
    networkPkgWindow->showOnlyInstalledPackages( false );

    networkPkgWindow->showUpgradedPackages( val );
}

void MainWindow :: setFilterCategory()
{
    if ( networkPkgWindow->setFilterCategory( ) )
        filter->setItemChecked( mnuFilterByCategory, true );
}

void MainWindow :: filterCategory()
{
    if ( filter->isItemChecked( mnuFilterByCategory ) )
    {
        networkPkgWindow->filterByCategory( false );
        filter->setItemChecked( mnuFilterByCategory, false );
    }
    else
    {
        if ( networkPkgWindow->filterByCategory( true ) )
            filter->setItemChecked( mnuFilterByCategory, true );
    }
}

void MainWindow :: raiseMainWidget()
{
    stack->raiseWidget( networkPkgWindow );
}

void MainWindow :: raiseProgressWidget()
{
    stack->raiseWidget( progressWindow );
}
