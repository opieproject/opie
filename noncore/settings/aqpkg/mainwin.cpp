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

#include <qpe/qpemenubar.h>
#include <qpe/qpetoolbar.h>
#include <qpe/resource.h>

#include <qaction.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qtimer.h>
#include <qwidgetstack.h>

#include "mainwin.h"
#include "progresswidget.h"
#include "datamgr.h"
#include "networkpkgmgr.h"
#include "settingsimpl.h"
#include "helpwindow.h"
#include "utils.h"
#include "global.h"

MainWindow :: MainWindow()
	:	QMainWindow( 0x0, 0x0, WStyle_ContextHelp )
{
    setCaption( tr( "AQPkg - Package Manager" ) );

    // Create UI widgets
    progressWindow = new ProgressWidget( this );
    networkPkgWindow = new NetworkPackageManager( this );

    // Build menu and tool bars
    setToolBarsMovable( FALSE );

    QPEToolBar *bar = new QPEToolBar( this );
    bar->setHorizontalStretchable( TRUE );
    QPEMenuBar *mb = new QPEMenuBar( bar );
    mb->setMargin( 0 );
    bar = new QPEToolBar( this );

    // Packages menu
    QPopupMenu *popup = new QPopupMenu( this );
    
    QAction *a = new QAction( tr( "Update lists" ), Resource::loadPixmap( "aqpkg/update" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to update package lists from servers." ) );
    connect( a, SIGNAL( activated() ), networkPkgWindow, SLOT( updateServer() ) );
    a->addTo( popup );
    a->addTo( bar );

    actionUpgrade = new QAction( tr( "Upgrade" ), Resource::loadPixmap( "aqpkg/upgrade" ), QString::null, 0, this, 0 );
    actionUpgrade->setWhatsThis( tr( "Click here to upgrade all installed packages if a newer version is available." ) );
    connect( actionUpgrade, SIGNAL( activated() ), networkPkgWindow, SLOT( upgradePackages() ) );
    actionUpgrade->addTo( popup );
    actionUpgrade->addTo( bar );

    iconDownload = Resource::loadPixmap( "aqpkg/download" );
    iconRemove = Resource::loadPixmap( "aqpkg/remove" );
    actionDownload = new QAction( tr( "Download" ), iconDownload, QString::null, 0, this, 0 );
    actionDownload->setWhatsThis( tr( "Click here to download the currently selected package(s)." ) );
    connect( actionDownload, SIGNAL( activated() ), networkPkgWindow, SLOT( downloadPackage() ) );
    actionDownload->addTo( popup );
    actionDownload->addTo( bar );

    a = new QAction( tr( "Apply changes" ), Resource::loadPixmap( "aqpkg/apply" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to install, remove or upgrade currently selected package(s)." ) );
    connect( a, SIGNAL( activated() ), networkPkgWindow, SLOT( applyChanges() ) );
    a->addTo( popup );
    a->addTo( bar );

    mb->insertItem( tr( "Packages" ), popup );

    // Search menu
    popup = new QPopupMenu( this );
    
    a = new QAction( tr( "Find" ), Resource::loadPixmap( "find" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to search for a specific package." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( searchForPackage() ) );
    a->addTo( popup );

    a = new QAction( tr( "Find next" ), Resource::loadPixmap( "next" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to search for the next package." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( repeatSearchForPackage() ) );
    a->addTo( popup );

    // Show 'quick jump' keypad?
    
    popup->insertSeparator();

    a = new QAction( tr( "Filter by category" ), Resource::loadPixmap( "aqpkg/filter" ),  QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to list packages belonging to one category." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( filterCategory() ) );
    a->addTo( popup );

    a = new QAction( tr( "Set filter category" ),  QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to change package category to used filter." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( setFilterCategory() ) );
    a->addTo( popup );

    mb->insertItem( tr( "Search" ), popup );

    
    // View menu
    popup = new QPopupMenu( this );

    a = new QAction( tr( "Show packages not installed" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to show packages available which have not been installed." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( filterUninstalledPackages() ) );
    a->addTo( popup );

    a = new QAction( tr( "Show installed packages" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to show packages currently installed on this device." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( filterInstalledPackages() ) );
    a->addTo( popup );

    a = new QAction( tr( "Show updated packages" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to show packages currently installed on this device which have a newer version available." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( filterUpgradedPackages() ) );
    a->addTo( popup );

    popup->insertSeparator();

    a = new QAction( tr( "Configure" ), Resource::loadPixmap( "aqpkg/config" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to configure this application." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( displaySettings() ) );
    a->addTo( popup );

    popup->insertSeparator();

    a = new QAction( tr( "Help" ), Resource::loadPixmap( "help_icon" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here for help." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( displayHelp() ) );
    a->addTo( popup );

    a = new QAction( tr( "About" ), Resource::loadPixmap( "UtilsIcon" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here for software version information." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( displayAbout() ) );
    a->addTo( popup );

    mb->insertItem( tr( "View" ), popup );

    // Create widget stack and add UI widgets
    stack = new QWidgetStack( this );
    stack->addWidget( progressWindow, 2 );
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
    connect( networkPkgWindow, SIGNAL( appRaiseMainWidget() ), this, SLOT( raiseMainWidget() ) );
    connect( networkPkgWindow, SIGNAL( appRaiseProgressWidget() ), this, SLOT( raiseProgressWidget() ) );
    connect( networkPkgWindow, SIGNAL( appEnableUpgrade( bool ) ), this, SLOT( enableUpgrade( bool ) ) );
    connect( networkPkgWindow, SIGNAL( appEnableDownload( bool ) ), this, SLOT( enableDownload( bool ) ) );
    connect( networkPkgWindow, SIGNAL( progressSetSteps( int ) ), progressWindow, SLOT( setSteps( int ) ) );
    connect( networkPkgWindow, SIGNAL( progressSetMessage( const QString & ) ),
             progressWindow, SLOT( setMessage( const QString & ) ) );
    connect( networkPkgWindow, SIGNAL( progressUpdate( int ) ), progressWindow, SLOT( update( int ) ) );
    
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

void MainWindow :: enableUpgrade( bool enabled )
{
    actionUpgrade->setEnabled( enabled );
}

void MainWindow :: enableDownload( bool enabled )
{
    if ( enabled )
    {
        actionDownload->setIconSet( iconDownload );
        actionDownload->setText( tr( "Download" ) );
        actionDownload->setWhatsThis( tr( "Click here to download the currently selected package(s)." ) );
    }
    else
    {
        actionDownload->setIconSet( iconRemove );
        actionDownload->setText( tr( "Remove" ) );
        actionDownload->setWhatsThis( tr( "Click here to uninstall the currently selected package(s)." ) );
    }
}
