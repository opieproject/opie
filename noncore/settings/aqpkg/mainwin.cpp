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

    setCaption( tr( "AQPkg - Package Manager" ) );

    // Create our menu
    help = new QPopupMenu( this );
    help->insertItem( "&General", this, SLOT(displayHelp()), Qt::CTRL+Qt::Key_G );
	help->insertItem( "&About", this, SLOT(displayAbout()), Qt::CTRL+Qt::Key_A );

    settings = new QPopupMenu( this );
	settings->insertItem( "&Settings", this, SLOT(displaySettings()), Qt::CTRL+Qt::Key_S );

    edit = new QPopupMenu( this );
	edit->insertItem( "&Find", this, SLOT(searchForPackage()), Qt::CTRL+Qt::Key_I );
	edit->insertItem( "Find &Next", this, SLOT(repeatSearchForPackage()), Qt::CTRL+Qt::Key_N );

    filter = new QPopupMenu( this );
	mnuShowUninstalledPkgsId = filter->insertItem( "Show &Non-Installed Packages", this, SLOT(filterUninstalledPackages()), Qt::CTRL+Qt::Key_U );
	mnuShowInstalledPkgsId = filter->insertItem( "Show In&stalled Packages", this, SLOT(filterInstalledPackages()), Qt::CTRL+Qt::Key_S );
	mnuShowUpgradedPkgsId = filter->insertItem( "Show U&pdated Packages", this, SLOT(filterUpgradedPackages()), Qt::CTRL+Qt::Key_P );
    filter->insertSeparator();
    mnuFilterByCategory = filter->insertItem( "Filter By &Category", this, SLOT(filterCategory()), Qt::CTRL+Qt::Key_C );
    mnuSetFilterCategory = filter->insertItem( "Set Filter C&ategory", this, SLOT(setFilterCategory()), Qt::CTRL+Qt::Key_A );

	// Create the main menu
	menu = menuBar();  //new QMenuBar( this );
	menu->insertItem( "&Settings", settings );
	menu->insertItem( "&Edit", edit );
	menu->insertItem( "&Filter", filter );
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
    QMessageBox::about( this, "About AQPkg", VERSION_TEXT );
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
