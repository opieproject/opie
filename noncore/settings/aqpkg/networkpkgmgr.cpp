/***************************************************************************
                          networkpkgmgr.cpp  -  description
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

#include <fstream>
#include <iostream>
using namespace std;

#include <unistd.h>
#include <stdlib.h>
#include <linux/limits.h>

#ifdef QWS
#include <qpe/qpeapplication.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/config.h>
#else
#include <qapplication.h>
#endif
#include <qlabel.h>
#include <qfile.h>
#include <qmessagebox.h>

#include "datamgr.h"
#include "networkpkgmgr.h"
#include "installdlgimpl.h"
#include "ipkg.h"
#include "inputdlg.h"
#include "letterpushbutton.h"

#include "global.h"

extern int compareVersions( const char *v1, const char *v2 );

NetworkPackageManager::NetworkPackageManager( DataManager *dataManager, QWidget *parent, const char *name)
  : QWidget(parent, name)
{
    dataMgr = dataManager;

#ifdef QWS
        // read download directory from config file
        Config cfg( "aqpkg" );
        cfg.setGroup( "settings" );
        currentlySelectedServer = cfg.readEntry( "selectedServer", "local" );
        showJumpTo = cfg.readBoolEntry( "showJumpTo", "true" );

#endif

    showUninstalledPkgs = false;
    showInstalledPkgs = false;
    showUpgradedPkgs = false;
    

    initGui();
    setupConnections();

    updateData();
}

NetworkPackageManager::~NetworkPackageManager()
{
}

void NetworkPackageManager :: timerEvent ( QTimerEvent * )
{
    killTimer( timerId );

    // Add server names to listbox
    updateData();
}

void NetworkPackageManager :: updateData()
{
	serversList->clear();
    packagesList->clear();

    
    vector<Server>::iterator it;
    int activeItem = -1;
    int i;
    for ( i = 0, it = dataMgr->getServerList().begin() ; it != dataMgr->getServerList().end() ; ++it, ++i )
    {
        if ( !it->isServerActive() )
        {
            i--;
            continue;
        }
        serversList->insertItem( it->getServerName() );
        if ( it->getServerName() == currentlySelectedServer )
        	activeItem = i;
	}

	// set selected server to be active server
	if ( activeItem != -1 )
		serversList->setCurrentItem( activeItem );
    serverSelected( 0 );
}

void NetworkPackageManager :: selectLocalPackage( const QString &pkg )
{
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
        if ( item->text().startsWith( pkg ) )
        {
            item->setOn( true );
            break;
        }
    }
}


void NetworkPackageManager :: initGui()
{
    QLabel *l = new QLabel( "Servers", this );
    serversList = new QComboBox( this );
    packagesList = new QListView( this );
    update = new QPushButton( "Refresh Lists", this );
    download = new QPushButton( "Download", this );
    upgrade = new QPushButton( "Upgrade", this );
    apply = new QPushButton( "Apply", this );

    QVBoxLayout *vbox = new QVBoxLayout( this, 0, -1, "VBox" );
    QHBoxLayout *hbox1 = new QHBoxLayout( vbox, -1, "HBox1" );
    hbox1->addWidget( l );
    hbox1->addWidget( serversList );

    QHBoxLayout *hbox3 = new QHBoxLayout( vbox, -1, "HBox1" );
    QHBoxLayout *hbox4 = new QHBoxLayout( vbox, -1, "HBox1" );


    if ( showJumpTo )
    {    
        char text[2];
        text[1] = '\0';
        for ( int i = 0 ; i < 26 ; ++i )
        {
                text[0] = 'A' + i;
                LetterPushButton *b = new LetterPushButton( text, this );
                connect( b, SIGNAL( released( QString ) ), this, SLOT( letterPushed( QString ) ) );
                if ( i < 13 )
                hbox3->addWidget( b );
                else
                hbox4->addWidget( b );
        }
    }

    vbox->addWidget( packagesList );
    packagesList->addColumn( "Packages" );

    QHBoxLayout *hbox2 = new QHBoxLayout( vbox, -1, "HBox2" );
    hbox2->addWidget( update );
    hbox2->addWidget( download );
    hbox2->addWidget( upgrade );
    hbox2->addWidget( apply );
}

void NetworkPackageManager :: setupConnections()
{
    connect( serversList, SIGNAL(activated( int )), this, SLOT(serverSelected( int )));
    connect( apply, SIGNAL(released()), this, SLOT(applyChanges()) );
    connect( download, SIGNAL(released()), this, SLOT(downloadPackage()) );
    connect( upgrade, SIGNAL( released()), this, SLOT(upgradePackages()) );
    connect( update, SIGNAL(released()), this, SLOT(updateServer()) );
}

void NetworkPackageManager :: showProgressDialog( char *initialText )
{
    if ( !progressDlg )
        progressDlg = new ProgressDlg( this, "Progress", false );
    progressDlg->setText( initialText );
    progressDlg->show();
}


void NetworkPackageManager :: serverSelected( int )
{
    packagesList->clear();

    // display packages
    QString serverName = serversList->currentText();
    currentlySelectedServer = serverName;

#ifdef QWS
        // read download directory from config file
        Config cfg( "aqpkg" );
        cfg.setGroup( "settings" );
        cfg.writeEntry( "selectedServer", currentlySelectedServer );
#endif

    Server *s = dataMgr->getServer( serverName );

    vector<Package> &list = s->getPackageList();
    vector<Package>::iterator it;
    for ( it = list.begin() ; it != list.end() ; ++it )
    {

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

        // If the local server, only display installed packages
        if ( serverName == LOCAL_SERVER && !it->isInstalled() )
            continue;


        text += it->getPackageName();
        if ( it->isInstalled() )
        {
            text += " (installed)";

            // If a different version of package is available, postfix it with an *
            if ( it->getVersion() != it->getInstalledVersion() )
            {

                if ( compareVersions( it->getInstalledVersion(), it->getVersion() ) == 1 )
                    text += "*";
            }
        }

        QCheckListItem *item = new QCheckListItem( packagesList, text, QCheckListItem::CheckBox );

        if ( it->isInstalled() )
        {
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
                new QCheckListItem( item, QString( "Installed To - " ) + destName );
        }
        
        if ( !it->isPackageStoredLocally() )
            new QCheckListItem( item, QString( "Description - " ) + it->getDescription() );
        else
            new QCheckListItem( item, QString( "Filename - " ) + it->getFilename() );
        
		if ( serverName == LOCAL_SERVER )
		{
        	new QCheckListItem( item, QString( "V. Installed - " ) + it->getVersion() );
		}
		else
		{
        	new QCheckListItem( item, QString( "V. Available - " ) + it->getVersion() );
        	if ( it->getLocalPackage() )
        	{
				if ( it->isInstalled() )
	            	new QCheckListItem( item, QString( "V. Installed - " ) + it->getInstalledVersion() );
			}
		}
        packagesList->insertItem( item );
    }

    // If the local server or the local ipkgs server disable the download button
    if ( serverName == LOCAL_SERVER )
    {
        upgrade->setEnabled( false );
        download->setText( "Download" );
        download->setEnabled( true );
    }
    else if ( serverName == LOCAL_IPKGS )
    {
        upgrade->setEnabled( false );
        download->setEnabled( true );
        download->setText( "Remove" );
    }
    else
    {
        upgrade->setEnabled( true );
        download->setEnabled( true );
        download->setText( "Download" );
    }
}

void NetworkPackageManager :: updateServer()
{
    QString serverName = serversList->currentText();

    // Update the current server
    // Display dialog
//  ProgressDlg *progDlg = new ProgressDlg( this );
//  QString status = "Updating package lists...";
//  progDlg->show();
//  progDlg->setText( status );

    // Disable buttons to stop silly people clicking lots on them :)

    // First, write out ipkg_conf file so that ipkg can use it
    dataMgr->writeOutIpkgConf();

    Ipkg ipkg;
    ipkg.setOption( "update" );

    InstallDlgImpl dlg( &ipkg, "Refreshing server package lists", this, "Upgrade", true );
    dlg.showDlg();

    // Reload data
    dataMgr->reloadServerData();
    serverSelected(-1);
//  delete progDlg;
}

void NetworkPackageManager :: upgradePackages()
{
    // We're gonna do an upgrade of all packages
    // First warn user that this isn't recommended
    QString text = "WARNING: Upgrading while\nOpie/Qtopia is running\nis NOT recommended!\n\nAre you sure?\n";
    QMessageBox warn("Warning", text, QMessageBox::Warning,
                        QMessageBox::Yes,
                        QMessageBox::No | QMessageBox::Escape | QMessageBox::Default ,
                        0, this );
    warn.adjustSize();
                        
    if ( warn.exec() == QMessageBox::Yes )
    {
        // First, write out ipkg_conf file so that ipkg can use it
        dataMgr->writeOutIpkgConf();

        // Now run upgrade
        Ipkg ipkg;
        ipkg.setOption( "upgrade" );
        
        InstallDlgImpl dlg( &ipkg, "Upgrading installed packages", this, "Upgrade", true );
        dlg.showDlg();

        // Reload data
        dataMgr->reloadServerData();
        serverSelected(-1);
    }
}


void NetworkPackageManager :: downloadPackage()
{
    bool doUpdate = true;
    if ( download->text() == "Download" )
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
    else if ( download->text() == "Remove" )
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
               
                Package *p = dataMgr->getServer( serversList->currentText() )->getPackage( name );

                QString msgtext;
                msgtext.sprintf( "Are you sure you wish to delete\n%s?", (const char *)p->getPackageName() );
                if ( QMessageBox::information( this, "Are you sure?",
                                    msgtext, "No", "Yes" ) == 1 )
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
        dataMgr->reloadServerData();
        serverSelected( -1 );
    }
}

void NetworkPackageManager :: downloadSelectedPackages()
{
    // First, write out ipkg_conf file so that ipkg can use it
    dataMgr->writeOutIpkgConf();

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
            QString name = item->text();
            int pos = name.find( "*" );
            name.truncate( pos );

            // if (there is a (installed), remove it
            pos = name.find( "(installed)" );
            if ( pos > 0 )
                name.truncate( pos - 1 );

            ipkg.setPackage( name );
            ipkg.runIpkg( );
        }
    }
}

void NetworkPackageManager :: downloadRemotePackage()
{
    // Display dialog
    bool ok;
    QString package = InputDialog::getText( "Install Remote Package", tr( "Enter package location" ), "http://", &ok, this );
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

    InstallDlgImpl dlg2( workingPackages, dataMgr, this, "Install", true );
    dlg2.showDlg();

    // Reload data
    dataMgr->reloadServerData();
    serverSelected(-1);

#ifdef QWS
    // Finally let the main system update itself
    QCopEnvelope e("QPE/System", "linkChanged(QString)");
    QString lf = QString::null;
    e << lf;
#endif
}


void NetworkPackageManager :: applyChanges()
{
    stickyOption = "";
    
    // First, write out ipkg_conf file so that ipkg can use it
    dataMgr->writeOutIpkgConf();

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
        QMessageBox::information( this, "Nothing to do",
                             "No packages selected", "OK" );
        
        return;
    }
    
    // do the stuff
    InstallDlgImpl dlg( workingPackages, dataMgr, this, "Install", true );
    dlg.showDlg();

    // Reload data
    dataMgr->reloadServerData();
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
InstallData NetworkPackageManager :: dealWithItem( QCheckListItem *item )
{
    QString name = item->text();
    int pos = name.find( "*" );
    name.truncate( pos );

    // if (there is a (installed), remove it
    pos = name.find( "(installed)" );
    if ( pos > 0 )
        name.truncate( pos - 1 );

    // Get package
    Server *s = dataMgr->getServer( serversList->currentText() );
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
            cout << "dest - " << p->getInstalledTo()->getDestinationName() << endl;
            cout << "dest - " << p->getInstalledTo()->getDestinationPath() << endl;
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
                caption = "Do you wish to remove or reinstall\n%s?";
                text = "Remove or ReInstall";
                secondButton = "ReInstall";
                secondOption = "R";
            }
            else if ( val == 1 )
            {
                // Version available is newer - option to remove or upgrade
                caption = "Do you wish to remove or upgrade\n%s?";
                text = "Remove or Upgrade";
                secondButton = "Upgrade";
                secondOption = "U";
            }

            // Sticky option not implemented yet, but will eventually allow
            // the user to say something like 'remove all'
            if ( stickyOption == "" )
            {
                QString msgtext;
                msgtext.sprintf( caption, (const char *)name );
                switch( QMessageBox::information( this, text,
                                    msgtext, "Remove", secondButton ) )
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

void NetworkPackageManager :: displayText( const QString &t )
{
    cout << t << endl;
}


void NetworkPackageManager :: letterPushed( QString t )
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


void NetworkPackageManager :: searchForPackage( bool findNext )
{
    bool ok = false;
    if ( !findNext || lastSearchText.isEmpty() )
        lastSearchText = InputDialog::getText( "Search for package", "Enter package to search for", lastSearchText, &ok, this ).lower();
    else
        ok = true;
        
    if ( ok && !lastSearchText.isEmpty() )
    {
        cout << "searching for " << lastSearchText << endl;
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
            cout << "checking " << item->text().lower() << endl;
            if ( item->text().lower().find( lastSearchText ) != -1 )
            {
                cout << "matched " << item->text() << endl;
                packagesList->ensureItemVisible( item );
                packagesList->setCurrentItem( item );
                break;
            }
        }   
    }
}

void NetworkPackageManager :: showOnlyUninstalledPackages( bool val )
{
    showUninstalledPkgs = val;
    serverSelected( -1 );
}

void NetworkPackageManager :: showOnlyInstalledPackages( bool val )
{
    showInstalledPkgs = val;
    serverSelected( -1 );
}

void NetworkPackageManager :: showUpgradedPackages( bool val )
{
    showUpgradedPkgs = val;
    serverSelected( -1 );
}
