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

#include <unistd.h>

#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpemenubar.h>
#include <qpe/qpeapplication.h>
#include <qpe/qpetoolbar.h>
#include <qpe/config.h>
#include <qpe/resource.h>

#include <qaction.h>
#include <qcombobox.h>
#include <qfile.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qprogressbar.h>
#include <qtimer.h>
#include <qwhatsthis.h>
#include <qwidgetstack.h>

#include "categoryfilterimpl.h"
#include "datamgr.h"
#include "global.h"
#include "helpwindow.h"
#include "inputdlg.h"
#include "installdlgimpl.h"
#include "letterpushbutton.h"
#include "mainwin.h"
#include "settingsimpl.h"
#include "utils.h"

extern int compareVersions( const char *v1, const char *v2 );

MainWindow :: MainWindow()
	:	QMainWindow( 0x0, 0x0, WStyle_ContextHelp )
{
    setCaption( tr( "AQPkg - Package Manager" ) );

#ifdef QWS
    // read download directory from config file
    Config cfg( "aqpkg" );
    cfg.setGroup( "settings" );
    currentlySelectedServer = cfg.readEntry( "selectedServer", "local" );
    showJumpTo = cfg.readBoolEntry( "showJumpTo", "true" );

#endif

    // Create UI widgets
    initMainWidget();
    initProgressWidget();
    
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
    connect( a, SIGNAL( activated() ), this, SLOT( updateServer() ) );
    a->addTo( popup );
    a->addTo( bar );

    actionUpgrade = new QAction( tr( "Upgrade" ), Resource::loadPixmap( "aqpkg/upgrade" ), QString::null, 0, this, 0 );
    actionUpgrade->setWhatsThis( tr( "Click here to upgrade all installed packages if a newer version is available." ) );
    connect( actionUpgrade, SIGNAL( activated() ), this, SLOT( upgradePackages() ) );
    actionUpgrade->addTo( popup );
    actionUpgrade->addTo( bar );

    iconDownload = Resource::loadPixmap( "aqpkg/download" );
    iconRemove = Resource::loadPixmap( "aqpkg/remove" );
    actionDownload = new QAction( tr( "Download" ), iconDownload, QString::null, 0, this, 0 );
    actionDownload->setWhatsThis( tr( "Click here to download the currently selected package(s)." ) );
    connect( actionDownload, SIGNAL( activated() ), this, SLOT( downloadPackage() ) );
    actionDownload->addTo( popup );
    actionDownload->addTo( bar );

    a = new QAction( tr( "Apply changes" ), Resource::loadPixmap( "aqpkg/apply" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to install, remove or upgrade currently selected package(s)." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( applyChanges() ) );
    a->addTo( popup );
    a->addTo( bar );

    mb->insertItem( tr( "Actions" ), popup );

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

    actionFilter = new QAction( tr( "Filter by category" ), Resource::loadPixmap( "aqpkg/filter" ),  QString::null, 0, this, 0 );
    actionFilter->setToggleAction( TRUE );
    actionFilter->setWhatsThis( tr( "Click here to list packages belonging to one category." ) );
    connect( actionFilter, SIGNAL( activated() ), this, SLOT( filterCategory() ) );
    actionFilter->addTo( popup );

    a = new QAction( tr( "Set filter category" ),  QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to change package category to used filter." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( setFilterCategory() ) );
    a->addTo( popup );

    popup->insertSeparator();

    a = new QAction( tr( "Find" ), Resource::loadPixmap( "find" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to search for text in package names." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( displayFindBar() ) );
    a->addTo( popup );

    actionFindNext = new QAction( tr( "Find next" ), Resource::loadIconSet( "next" ), QString::null, 0, this, 0 );
    actionFindNext->setEnabled( FALSE );
    actionFindNext->setWhatsThis( tr( "Click here to find the next package name containing the text you are searching for." ) );
    connect( actionFindNext, SIGNAL( activated() ), this, SLOT( repeatFind() ) );
    actionFindNext->addTo( popup );
    actionFindNext->addTo( findBar );

    mb->insertItem( tr( "View" ), popup );

    
    // Options menu
    popup = new QPopupMenu( this );

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

    mb->insertItem( tr( "Options" ), popup );
    
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

void MainWindow :: initMainWidget()
{
    networkPkgWindow = new QWidget( this );
    
    QLabel *l = new QLabel( tr( "Servers:" ), networkPkgWindow );
    
    serversList = new QComboBox( networkPkgWindow );
    connect( serversList, SIGNAL( activated( int ) ), this, SLOT( serverSelected( int ) ) );
    QWhatsThis::add( serversList, tr( "Click here to select a package feed." ) );
    
    installedIcon = Resource::loadPixmap( "aqpkg/installed" );
    updatedIcon = Resource::loadPixmap( "aqpkg/updated" );
    
    packagesList = new QListView( networkPkgWindow );
    packagesList->addColumn( tr( "Packages" ), 225 );
    QWhatsThis::add( packagesList, tr( "This is a listing of all packages for the server feed selected above.\n\nA blue dot next to the package name indicates that the package is currently installed.\n\nA blue dot with a star indicates that a newer version of the package is available from the server feed.\n\nClick inside the box at the left to select a package." ) );

    QVBoxLayout *vbox = new QVBoxLayout( networkPkgWindow, 0, -1 );
    QHBoxLayout *hbox1 = new QHBoxLayout( vbox, -1 );
    hbox1->addWidget( l );
    hbox1->addWidget( serversList );


    QHBoxLayout *hbox3 = new QHBoxLayout( vbox, -1 );
    QHBoxLayout *hbox4 = new QHBoxLayout( vbox, -1 );

    if ( showJumpTo )
    {    
        char text[2];
        text[1] = '\0';
        for ( int i = 0 ; i < 26 ; ++i )
        {
                text[0] = 'A' + i;
                LetterPushButton *b = new LetterPushButton( text, networkPkgWindow );
                connect( b, SIGNAL( released( QString ) ), this, SLOT( letterPushed( QString ) ) );
                if ( i < 13 )
                hbox3->addWidget( b );
                else
                hbox4->addWidget( b );
        }
    }
    
    vbox->addWidget( packagesList );

    downloadEnabled = TRUE;
}

void MainWindow :: initProgressWidget()
{
    progressWindow = new QWidget( this );
    
    QVBoxLayout *layout = new QVBoxLayout( progressWindow, 4, 4 );

    m_status = new QLabel( progressWindow );
    m_status->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
    layout->addWidget( m_status );

    m_progress = new QProgressBar( progressWindow );
    layout->addWidget( m_progress );
}

void MainWindow :: init()
{
    stack->raiseWidget( progressWindow );
    
    mgr = new DataManager();
    connect( mgr, SIGNAL( progressSetSteps( int ) ), this, SLOT( setProgressSteps( int ) ) );
    connect( mgr, SIGNAL( progressSetMessage( const QString & ) ),
             this, SLOT( setProgressMessage( const QString & ) ) );
    connect( mgr, SIGNAL( progressUpdate( int ) ), this, SLOT( updateProgress( int ) ) );
    mgr->loadServers();
    
    showUninstalledPkgs = false;
    showInstalledPkgs = false;
    showUpgradedPkgs = false;
    categoryFilterEnabled = false;

    updateData();
    
    stack->raiseWidget( networkPkgWindow );
}

void MainWindow :: setDocument( const QString &doc )
{
    // Remove path from package
    QString package = Utils::getPackageNameFromIpkFilename( doc );
//    std::cout << "Selecting package " << package << std::endl;
    
    // First select local server
    for ( int i = 0 ; i < serversList->count() ; ++i )
    {
        if ( serversList->text( i ) == LOCAL_IPKGS )
        {
            serversList->setCurrentItem( i );
            break;
        }
    }
    serverSelected( 0 );

    // Now set the check box of the selected package
    for ( QCheckListItem *item = (QCheckListItem *)packagesList->firstChild();
          item != 0 ;
          item = (QCheckListItem *)item->nextSibling() )
    {
        if ( item->text().startsWith( package ) )
        {
            item->setOn( true );
            break;
        }
    }
}

void MainWindow :: displaySettings()
{
    SettingsImpl *dlg = new SettingsImpl( mgr, this, "Settings", true );
    if ( dlg->showDlg( 0 ) )
    	updateData();
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
    searchForPackage( findEdit->text() );
}

void MainWindow :: findPackage( const QString &text )
{
    actionFindNext->setEnabled( !text.isEmpty() );
    searchForPackage( text );
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
    showUninstalledPkgs = actionUninstalled->isOn();
    if ( showUninstalledPkgs )
    {
        showInstalledPkgs = FALSE;
        showUpgradedPkgs = FALSE;
    }
    serverSelected( -1 );
    
    actionInstalled->setOn( FALSE );
    actionUpdated->setOn( FALSE );
}

void MainWindow :: filterInstalledPackages()
{
    showInstalledPkgs = actionInstalled->isOn();
    if ( showInstalledPkgs )
    {
        showUninstalledPkgs = FALSE;
        showUpgradedPkgs = FALSE;
    }
    serverSelected( -1 );
    
    actionUninstalled->setOn( FALSE );
    actionUpdated->setOn( FALSE );
}

void MainWindow :: filterUpgradedPackages()
{
    showUpgradedPkgs = actionUpdated->isOn();
    if ( showUpgradedPkgs )
    {
        showUninstalledPkgs = FALSE;
        showInstalledPkgs = FALSE;
    }
    serverSelected( -1 );
    
    actionUninstalled->setOn( FALSE );
    actionInstalled->setOn( FALSE );
}

bool MainWindow :: setFilterCategory()
{
    // Get categories;
    CategoryFilterImpl dlg( DataManager::getAvailableCategories(), categoryFilter, this );
    if ( dlg.exec() == QDialog::Accepted )
    {
        categoryFilter = dlg.getSelectedFilter();

        if ( categoryFilter == "" )
            return false;
            
        categoryFilterEnabled = true;
        serverSelected( -1 );
        actionFilter->setOn( TRUE );
        return true;
    }

    return false;
}

void MainWindow :: filterCategory()
{
    if ( !actionFilter->isOn() )
    {
        filterByCategory( FALSE );
    }
    else
    {
        actionFilter->setOn( filterByCategory( TRUE ) );
    }
}

bool MainWindow :: filterByCategory( bool val )
{
    if ( val )
    {
        if ( categoryFilter == "" )
        {
            if ( !setFilterCategory() )
                return false;
        }
            
        categoryFilterEnabled = true;
        serverSelected( -1 );
        return true;
    }
    else
    {
        // Turn off filter
        categoryFilterEnabled = false;
        serverSelected( -1 );
        return false;
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

void MainWindow :: setProgressSteps( int numsteps )
{
    m_progress->setTotalSteps( numsteps );
}

void MainWindow :: setProgressMessage( const QString &msg )
{
    m_status->setText( msg );
}

void MainWindow :: updateProgress( int progress )
{
    m_progress->setProgress( progress );
}

void MainWindow :: updateData()
{
    m_progress->setTotalSteps( mgr->getServerList().size() );
    
    serversList->clear();
    packagesList->clear();

    vector<Server>::iterator it;
    int activeItem = -1;
    int i;
    QString serverName;
    for ( i = 0, it = mgr->getServerList().begin() ; it != mgr->getServerList().end() ; ++it, ++i )
    {
        serverName = it->getServerName();
        m_status->setText( tr( "Building server list:\n\t%1" ).arg( serverName ) );
        m_progress->setProgress( i );
        qApp->processEvents();
        
//        cout << "Adding " << it->getServerName() << " to combobox" << endl;
        if ( !it->isServerActive() )
        {
//            cout << serverName << " is not active" << endl;
            i--;
            continue;
        }
        
        serversList->insertItem( serverName );
        if ( serverName == currentlySelectedServer )
        	activeItem = i;
	}

	// set selected server to be active server
	if ( activeItem != -1 )
		serversList->setCurrentItem( activeItem );
    serverSelected( 0, FALSE );
}

void MainWindow :: serverSelected( int index )
{
    serverSelected( index, TRUE );
}

void MainWindow :: serverSelected( int, bool raiseProgress )
{
    QPixmap nullIcon( installedIcon.size() );
    nullIcon.fill( colorGroup().base() );
    
    // display packages
    QString serverName = serversList->currentText();
    currentlySelectedServer = serverName;

    vector<Server>::iterator s = mgr->getServer( serverName );

    vector<Package> &list = s->getPackageList();
    vector<Package>::iterator it;
    
    // Display progress widget while loading list
    bool doProgress = ( list.size() > 200 );
    if ( doProgress )
    {
        if ( raiseProgress )
        {
            stack->raiseWidget( progressWindow );
        }
        m_progress->setTotalSteps( list.size() );
        m_status->setText( tr( "Building package list for:\n\t%1" ).arg( serverName ) );
    }

    packagesList->clear();

#ifdef QWS
        // read download directory from config file
        Config cfg( "aqpkg" );
        cfg.setGroup( "settings" );
        cfg.writeEntry( "selectedServer", currentlySelectedServer );
#endif

    int i = 0;
    for ( it = list.begin() ; it != list.end() ; ++it )
    {
        // Update progress after every 100th package (arbitrary value, seems to give good balance)
        i++;
        if ( ( i % 100 ) == 0 )
        {
            if ( doProgress )
            {
                m_progress->setProgress( i );
            }
            qApp->processEvents();
        }
        
        QString text = "";

        // Apply show only uninstalled packages filter
        if ( showUninstalledPkgs && it->isInstalled() )
            continue;

        // Apply show only installed packages filter
        if ( showInstalledPkgs && !it->isInstalled() )
            continue;

        // Apply show only new installed packages filter
        if ( showUpgradedPkgs )
        {
            if ( !it->isInstalled() ||
                 compareVersions( it->getInstalledVersion(), it->getVersion() ) != 1 )
                continue;
        }

        // Apply the section filter
        if ( categoryFilterEnabled && categoryFilter != "" )
        {
            if ( it->getSection() == "" || categoryFilter.find( it->getSection().lower() ) == -1 )
                continue;
        }

        // If the local server, only display installed packages
        if ( serverName == LOCAL_SERVER && !it->isInstalled() )
            continue;


        QCheckListItem *item = new QCheckListItem( packagesList, it->getPackageName(), QCheckListItem::CheckBox );
        
        if ( it->isInstalled() )
        {
            // If a different version of package is available, show update available icon
            // Otherwise, show installed icon
            if ( it->getVersion() != it->getInstalledVersion() &&
                 compareVersions( it->getInstalledVersion(), it->getVersion() ) == 1)
            {

                item->setPixmap( 0, updatedIcon );
            }
            else
            {
                item->setPixmap( 0, installedIcon );
            }
            
            QString destName = "";
            if ( it->getLocalPackage() )
            {
                if ( it->getLocalPackage()->getInstalledTo() )
                    destName = it->getLocalPackage()->getInstalledTo()->getDestinationName();
            }
            else
            {
                if ( it->getInstalledTo() )
                    destName = it->getInstalledTo()->getDestinationName();
            }
            if ( destName != "" )
                new QCheckListItem( item, QString( tr( "Installed To - %1" ).arg( destName ) ) );
        }
        else
        {
            item->setPixmap( 0, nullIcon );
        }
        
        if ( !it->isPackageStoredLocally() )
        {
            new QCheckListItem( item, QString( tr( "Description - %1" ).arg( it->getDescription() ) ) );
            new QCheckListItem( item, QString( tr( "Size - %1" ).arg( it->getPackageSize() ) ) );
            new QCheckListItem( item, QString( tr( "Section - %1" ).arg( it->getSection() ) ) );
                    }
        else
            new QCheckListItem( item, QString( tr( "Filename - %1" ).arg( it->getFilename() ) ) );
        
		if ( serverName == LOCAL_SERVER )
		{
        	new QCheckListItem( item, QString( tr( "V. Installed - %1" ).arg( it->getVersion() ) ) );
		}
		else
		{
        	new QCheckListItem( item, QString( tr( "V. Available - %1" ).arg( it->getVersion() ) ) );
        	if ( it->getLocalPackage() )
        	{
				if ( it->isInstalled() )
	            	new QCheckListItem( item, QString( tr( "V. Installed - %1" ).arg( it->getInstalledVersion() ) ) );
			}
		}

        packagesList->insertItem( item );
    }

    // If the local server or the local ipkgs server disable the download button
    if ( serverName == LOCAL_SERVER )
    {
        downloadEnabled = TRUE;
        actionUpgrade->setEnabled( FALSE );
    }
    else if ( serverName == LOCAL_IPKGS )
    {
        downloadEnabled = FALSE;
        actionUpgrade->setEnabled( FALSE );
    }
    else
    {
        downloadEnabled = TRUE;
        actionUpgrade->setEnabled( TRUE );
    }
    enableDownload( downloadEnabled );

    // Display this widget once everything is done
    if ( doProgress && raiseProgress )
    {
        stack->raiseWidget( networkPkgWindow );
    }
}

void MainWindow :: searchForPackage( const QString &text )
{
    if ( !text.isEmpty() )
    {
//        cout << "searching for " << text << endl;
        // look through package list for text startng at current position
        vector<InstallData> workingPackages;
        QCheckListItem *start = (QCheckListItem *)packagesList->currentItem();
        if ( start != 0 )
            start = (QCheckListItem *)start->nextSibling();
        
        if ( start == 0 )
            start = (QCheckListItem *)packagesList->firstChild();
        
        for ( QCheckListItem *item = start; item != 0 ;
              item = (QCheckListItem *)item->nextSibling() )
        {
//            cout << "checking " << item->text().lower() << endl;
            if ( item->text().lower().find( text ) != -1 )
            {
//                cout << "matched " << item->text() << endl;
                packagesList->ensureItemVisible( item );
                packagesList->setCurrentItem( item );
                break;
            }
        }   
    }
}

void MainWindow :: updateServer()
{
    QString serverName = serversList->currentText();

    // Update the current server
    // Display dialog

    // Disable buttons to stop silly people clicking lots on them :)

    // First, write out ipkg_conf file so that ipkg can use it
    mgr->writeOutIpkgConf();

    Ipkg ipkg;
    ipkg.setOption( "update" );

    InstallDlgImpl dlg( &ipkg, tr( "Refreshing server package lists" ), this, tr( "Upgrade" ), true );
    dlg.showDlg();

    // Reload data
    mgr->reloadServerData();
    serverSelected(-1);
//  delete progDlg;
}

void MainWindow :: upgradePackages()
{
    // We're gonna do an upgrade of all packages
    // First warn user that this isn't recommended
    // TODO - ODevice????
    QString text = tr( "WARNING: Upgrading while\nOpie/Qtopia is running\nis NOT recommended!\n\nAre you sure?\n" );
    QMessageBox warn( tr( "Warning" ), text, QMessageBox::Warning,
                        QMessageBox::Yes,
                        QMessageBox::No | QMessageBox::Escape | QMessageBox::Default ,
                        0, this );
    warn.adjustSize();
                        
    if ( warn.exec() == QMessageBox::Yes )
    {
        // First, write out ipkg_conf file so that ipkg can use it
        mgr->writeOutIpkgConf();

        // Now run upgrade
        Ipkg ipkg;
        ipkg.setOption( "upgrade" );
        
        InstallDlgImpl dlg( &ipkg, tr( "Upgrading installed packages" ), this, tr( "Upgrade" ), true );
        dlg.showDlg();

        // Reload data
        mgr->reloadServerData();
        serverSelected(-1);
    }
}
                                                       void MainWindow :: downloadPackage()
{
    bool doUpdate = true;
    if ( downloadEnabled )
    {
        // See if any packages are selected
        bool found = false;
        if ( serversList->currentText() != LOCAL_SERVER )
        {
            for ( QCheckListItem *item = (QCheckListItem *)packagesList->firstChild();
                  item != 0 && !found;
                  item = (QCheckListItem *)item->nextSibling() )
            {
                if ( item->isOn() )
                    found = true;
            }
        }
        
        // If user selected some packages then download the and store the locally
        // otherwise, display dialog asking user what package to download from an http server
        // and whether to install it
        if ( found )
            downloadSelectedPackages();
        else
            downloadRemotePackage();
        
    }
    else
    {
        doUpdate = false;
        for ( QCheckListItem *item = (QCheckListItem *)packagesList->firstChild();
              item != 0 ;
              item = (QCheckListItem *)item->nextSibling() )
        {
            if ( item->isOn() )
            {
                QString name = item->text();
                int pos = name.find( "*" );
                name.truncate( pos );

                // if (there is a (installed), remove it
                pos = name.find( "(installed)" );
                if ( pos > 0 )
                    name.truncate( pos - 1 );
               
                Package *p = mgr->getServer( serversList->currentText() )->getPackage( name );

                QString msgtext;
                msgtext = tr( "Are you sure you wish to delete\n%1?" ).arg( (const char *)p->getPackageName() );
                if ( QMessageBox::information( this, tr( "Are you sure?" ),
                                    msgtext, tr( "No" ), tr( "Yes" ) ) == 1 )
                {
                    doUpdate = true;
                    QFile f( p->getFilename() );
                    f.remove();
                }
            }
        }
    }

    if ( doUpdate )
    {
        mgr->reloadServerData();
        serverSelected( -1 );
    }
}

void MainWindow :: downloadSelectedPackages()
{
    // First, write out ipkg_conf file so that ipkg can use it
    mgr->writeOutIpkgConf();

    // Display dialog to user asking where to download the files to
    bool ok = FALSE;
    QString dir = "";
#ifdef QWS
    // read download directory from config file
    Config cfg( "aqpkg" );
    cfg.setGroup( "settings" );
    dir = cfg.readEntry( "downloadDir", "/home/root/Documents/application/ipkg" );
#endif

    QString text = InputDialog::getText( tr( "Download to where" ), tr( "Enter path to download to" ), dir, &ok, this );
    if ( ok && !text.isEmpty() )
        dir = text;   // user entered something and pressed ok
    else
        return; // user entered nothing or pressed cancel

#ifdef QWS
    // Store download directory in config file
    cfg.writeEntry( "downloadDir", dir );
#endif

    // Get starting directory
    char initDir[PATH_MAX];
    getcwd( initDir, PATH_MAX );

    // Download each package
    Ipkg ipkg;
    connect( &ipkg, SIGNAL(outputText(const QString &)), this, SLOT(displayText(const QString &)));

    ipkg.setOption( "download" );
    ipkg.setRuntimeDirectory( dir );
    for ( QCheckListItem *item = (QCheckListItem *)packagesList->firstChild();
          item != 0 ;
          item = (QCheckListItem *)item->nextSibling() )
    {
        if ( item->isOn() )
        {
            ipkg.setPackage( item->text() );
            ipkg.runIpkg( );
        }
    }
}

void MainWindow :: downloadRemotePackage()
{
    // Display dialog
    bool ok;
    QString package = InputDialog::getText( tr( "Install Remote Package" ), tr( "Enter package location" ), "http://", &ok, this );
    if ( !ok || package.isEmpty() )
        return;
//    DownloadRemoteDlgImpl dlg( this, "Install", true );
//    if ( dlg.exec() == QDialog::Rejected )
//        return;

    // grab details from dialog
//    QString package = dlg.getPackageLocation();

    InstallData item;
    item.option = "I";
    item.packageName = package;
    vector<InstallData> workingPackages;
    workingPackages.push_back( item );

    InstallDlgImpl dlg2( workingPackages, mgr, this, "Install", true );
    dlg2.showDlg();

    // Reload data
    mgr->reloadServerData();
    serverSelected(-1);

#ifdef QWS
    // Finally let the main system update itself
    QCopEnvelope e("QPE/System", "linkChanged(QString)");
    QString lf = QString::null;
    e << lf;
#endif
}


void MainWindow :: applyChanges()
{
    stickyOption = "";
    
    // First, write out ipkg_conf file so that ipkg can use it
    mgr->writeOutIpkgConf();

    // Now for each selected item
    // deal with it

    vector<InstallData> workingPackages;
    for ( QCheckListItem *item = (QCheckListItem *)packagesList->firstChild();
          item != 0 ;
          item = (QCheckListItem *)item->nextSibling() )
    {
        if ( item->isOn() )
        {
            InstallData data = dealWithItem( item );
            workingPackages.push_back( data );
		}
    }

    if ( workingPackages.size() == 0 )
    {
        // Nothing to do
        QMessageBox::information( this, tr( "Nothing to do" ),
                             tr( "No packages selected" ), tr( "OK" ) );
        
        return;
    }
    
    // do the stuff
    InstallDlgImpl dlg( workingPackages, mgr, this, "Install", true );
    dlg.showDlg();

    // Reload data
    mgr->reloadServerData();
    serverSelected(-1);

#ifdef QWS
    // Finally let the main system update itself
    QCopEnvelope e("QPE/System", "linkChanged(QString)");
    QString lf = QString::null;
    e << lf;
#endif
}

// decide what to do - either remove, upgrade or install
// Current rules:
//    If not installed - install
//    If installed and different version available - upgrade
//    If installed and version up to date - remove
InstallData MainWindow :: dealWithItem( QCheckListItem *item )
{
    QString name = item->text();

    // Get package
    vector<Server>::iterator s = mgr->getServer( serversList->currentText() );
    Package *p = s->getPackage( name );

    // If the package has a filename then it is a local file
    if ( p->isPackageStoredLocally() )
        name = p->getFilename();

    QString option;
    QString dest = "root";
    if ( !p->isInstalled() )
    {
        InstallData item;
        item.option = "I";
        item.packageName = name;
        return item;
    }
    else
    {
        InstallData item;
        item.option = "D";
        if ( !p->isPackageStoredLocally() )
            item.packageName = p->getInstalledPackageName();
        else
            item.packageName = name;
        
        if ( p->getInstalledTo() )
        {
            item.destination = p->getInstalledTo();
//            cout << "dest - " << p->getInstalledTo()->getDestinationName() << endl;
//            cout << "dest - " << p->getInstalledTo()->getDestinationPath() << endl;
        }
        else
        {
            item.destination = p->getLocalPackage()->getInstalledTo();
        }

        // Now see if version is newer or not
        int val = compareVersions( p->getInstalledVersion(), p->getVersion() );

        // If the version requested is older and user selected a local ipk file, then reinstall the file
        if ( p->isPackageStoredLocally() && val == -1 )
            val = 0;
            
        if ( val == -2 )
        {
            // Error - should handle
        }
        else if ( val == -1 )
        {
            // Version available is older - remove only
            item.option = "D";
        }
        else
        {
            QString caption;
            QString text;
            QString secondButton;
            QString secondOption;
            if ( val == 0 )
            {
                // Version available is the same - option to remove or reinstall
                caption = tr( "Do you wish to remove or reinstall\n%1?" );
                text = tr( "Remove or ReInstall" );
                secondButton = tr( "ReInstall" );
                secondOption = tr( "R" );
            }
            else if ( val == 1 )
            {
                // Version available is newer - option to remove or upgrade
                caption = tr( "Do you wish to remove or upgrade\n%1?" );
                text = tr( "Remove or Upgrade" );
                secondButton = tr( "Upgrade" );
                secondOption = tr( "U" );
            }

            // Sticky option not implemented yet, but will eventually allow
            // the user to say something like 'remove all'
            if ( stickyOption == "" )
            {
                QString msgtext;
                msgtext = caption.arg( ( const char * )name );
                switch( QMessageBox::information( this, text,
                                    msgtext, tr( "Remove" ), secondButton ) )
                {
                    case 0: // Try again or Enter
                        // option 0 = Remove
                        item.option = "D";
                        break;
                    case 1: // Quit or Escape
                        item.option = secondOption;
                        break;
                }
            }
            else
            {
//                item.option = stickyOption;
            }
        }

        
        // Check if we are reinstalling the same version
        if ( item.option != "R" )
           item.recreateLinks = true;
        else
           item.recreateLinks = false;

        // User hit cancel (on dlg - assume remove)
        return item;
    }
}

void MainWindow :: letterPushed( QString t )
{
    QCheckListItem *top = (QCheckListItem *)packagesList->firstChild();
    QCheckListItem *start = (QCheckListItem *)packagesList->currentItem();
    if ( packagesList->firstChild() == 0 )
        return;

    QCheckListItem *item;
    if ( start == 0 )
    {
        item = (QCheckListItem *)packagesList->firstChild();
        start = top;
    }
    else
        item = (QCheckListItem *)start->nextSibling();

    if ( item == 0 )
        item = (QCheckListItem *)packagesList->firstChild();
    do
    {
        if ( item->text().lower().startsWith( t.lower() ) )
        {
            packagesList->setSelected( item, true );
            packagesList->ensureItemVisible( item );
            break;
        }

        item = (QCheckListItem *)item->nextSibling();
        if ( !item )
            item = (QCheckListItem *)packagesList->firstChild();        
    } while ( item != start);
}
