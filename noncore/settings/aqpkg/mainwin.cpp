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
#include <qlineedit.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qtimer.h>
#include <qwhatsthis.h>
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

    // Find toolbar
    findBar = new QPEToolBar( this );
    addToolBar( findBar, QMainWindow::Top, true );
    findBar->setHorizontalStretchable( true );
    findEdit = new QLineEdit( findBar );
    QWhatsThis::add( findEdit, tr( "Type the text to search for here." ) );
    findBar->setStretchableWidget( findEdit );
    connect( findEdit, SIGNAL( textChanged( const QString & ) ), this, SLOT( findPackage( const QString & ) ) );
    
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
    a->setWhatsThis( tr( "Click here to search for text in package names." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( displayFindBar() ) );
    a->addTo( popup );

    actionFindNext = new QAction( tr( "Find next" ), Resource::loadIconSet( "next" ), QString::null, 0, this, 0 );
    actionFindNext->setEnabled( FALSE );
    actionFindNext->setWhatsThis( tr( "Click here to search for the package name containing the text you are searching for." ) );
    connect( actionFindNext, SIGNAL( activated() ), this, SLOT( repeatFind() ) );
    actionFindNext->addTo( popup );
    actionFindNext->addTo( findBar );

    // Show 'quick jump' keypad?
    
    popup->insertSeparator();

    actionFilter = new QAction( tr( "Filter by category" ), Resource::loadPixmap( "aqpkg/filter" ),  QString::null, 0, this, 0 );
    actionFilter->setToggleAction( TRUE );
    actionFilter->setWhatsThis( tr( "Click here to list packages belonging to one category." ) );
    connect( actionFilter, SIGNAL( activated() ), this, SLOT( filterCategory() ) );
    actionFilter->addTo( popup );

    a = new QAction( tr( "Set filter category" ),  QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to change package category to used filter." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( setFilterCategory() ) );
    a->addTo( popup );

    mb->insertItem( tr( "Search" ), popup );

    
    // View menu
    popup = new QPopupMenu( this );

    actionUninstalled = new QAction( tr( "Show packages not installed" ), QString::null, 0, this, 0 );
    actionUninstalled->setToggleAction( TRUE );
    actionUninstalled->setWhatsThis( tr( "Click here to show packages available which have not been installed." ) );
    connect( actionUninstalled, SIGNAL( activated() ), this, SLOT( filterUninstalledPackages() ) );
    actionUninstalled->addTo( popup );

    actionInstalled = new QAction( tr( "Show installed packages" ), QString::null, 0, this, 0 );
    actionInstalled->setToggleAction( TRUE );
    actionInstalled->setWhatsThis( tr( "Click here to show packages currently installed on this device." ) );
    connect( actionInstalled, SIGNAL( activated() ), this, SLOT( filterInstalledPackages() ) );
    actionInstalled->addTo( popup );

    actionUpdated = new QAction( tr( "Show updated packages" ), QString::null, 0, this, 0 );
    actionUpdated->setToggleAction( TRUE );
    actionUpdated->setWhatsThis( tr( "Click here to show packages currently installed on this device which have a newer version available." ) );
    connect( actionUpdated, SIGNAL( activated() ), this, SLOT( filterUpgradedPackages() ) );
    actionUpdated->addTo( popup );

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
    
    // Finish find toolbar creation
    a = new QAction( QString::null, Resource::loadPixmap( "close" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to hide the find toolbar." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( hideFindBar() ) );
    a->addTo( findBar );
    findBar->hide();

    
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
    
void MainWindow :: displayFindBar()
{
    findBar->show();
    findEdit->setFocus();
}

void MainWindow :: repeatFind()
{
    networkPkgWindow->searchForPackage( findEdit->text() );
}

void MainWindow :: findPackage( const QString &text )
{
    actionFindNext->setEnabled( !text.isEmpty() );
    networkPkgWindow->searchForPackage( text );
}

void MainWindow :: hideFindBar()
{
    findBar->hide();
}

void MainWindow :: displayAbout()
{
    QMessageBox::about( this, tr( "About AQPkg" ), tr( VERSION_TEXT ) );
}

void MainWindow :: filterUninstalledPackages()
{
    networkPkgWindow->showOnlyUninstalledPackages( actionUninstalled->isOn() );
    actionInstalled->setOn( FALSE );
    actionUpdated->setOn( FALSE );
}

void MainWindow :: filterInstalledPackages()
{
    actionUninstalled->setOn( FALSE );
    networkPkgWindow->showOnlyInstalledPackages( actionInstalled->isOn() );
    actionUpdated->setOn( FALSE );
}

void MainWindow :: filterUpgradedPackages()
{
    actionUninstalled->setOn( FALSE );
    actionInstalled->setOn( FALSE );
    networkPkgWindow->showUpgradedPackages( actionUpdated->isOn() );
}

void MainWindow :: setFilterCategory()
{
    if ( networkPkgWindow->setFilterCategory() )
        actionFilter->setOn( TRUE );
}

void MainWindow :: filterCategory()
{
    if ( !actionFilter->isOn() )
    {
        networkPkgWindow->filterByCategory( FALSE );
    }
    else
    {
        actionFilter->setOn( networkPkgWindow->filterByCategory( TRUE ) );
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
