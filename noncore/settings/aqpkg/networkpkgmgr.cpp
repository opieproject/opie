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

#include "datamgr.h"
#include "networkpkgmgr.h"
#include "installdlgimpl.h"
#include "ipkg.h"
#include "inputdlg.h"

#include "global.h"

NetworkPackageManager::NetworkPackageManager( DataManager *dataManager, QWidget *parent, const char *name)
  : QWidget(parent, name)
{
    dataMgr = dataManager;

    initGui();
    setupConnections();

    progressDlg = 0;
    timerId = startTimer( 100 );
}

NetworkPackageManager::~NetworkPackageManager()
{
}

void NetworkPackageManager :: timerEvent ( QTimerEvent * )
{
    killTimer( timerId );

//    showProgressDialog();
    // Add server names to listbox
    updateData();

//    progressDlg->hide();
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
        serversList->insertItem( it->getServerName() );
        if ( it->getServerName() == dataMgr->getActiveServer() )
        	activeItem = i;
	}

	// set selected server to be active server
	if ( activeItem != -1 )
		serversList->setCurrentItem( activeItem );
    serverSelected( 0 );
}


void NetworkPackageManager :: initGui()
{
    QLabel *l = new QLabel( "Servers", this );
    serversList = new QComboBox( this );
    packagesList = new QListView( this );
    update = new QPushButton( "Refresh List", this );
    download = new QPushButton( "Download", this );
    apply = new QPushButton( "Apply", this );

    QVBoxLayout *vbox = new QVBoxLayout( this, 0, -1, "VBox" );
    QHBoxLayout *hbox1 = new QHBoxLayout( vbox, -1, "HBox1" );
    hbox1->addWidget( l );
    hbox1->addWidget( serversList );

    vbox->addWidget( packagesList );
    packagesList->addColumn( "Packages" );

    QHBoxLayout *hbox2 = new QHBoxLayout( vbox, -1, "HBox2" );
    hbox2->addWidget( update );
    hbox2->addWidget( download );
    hbox2->addWidget( apply );
}

void NetworkPackageManager :: setupConnections()
{
    connect( serversList, SIGNAL(activated( int )), this, SLOT(serverSelected( int )));
    connect( apply, SIGNAL(released()), this, SLOT(applyChanges()) );
    connect( download, SIGNAL(released()), this, SLOT(downloadPackage()) );
    connect( update, SIGNAL(released()), this, SLOT(updateServer()) );
}

void NetworkPackageManager :: showProgressDialog()
{
    if ( !progressDlg )
        progressDlg = new ProgressDlg( this, "Progress", false );
    progressDlg->setText( "Reading installed packages" );
    progressDlg->show();
}


void NetworkPackageManager :: serverSelected( int )
{
    packagesList->clear();

    // display packages
    QString serverName = serversList->currentText();
    Server *s = dataMgr->getServer( serverName );
    dataMgr->setActiveServer( serverName );

    vector<Package> &list = s->getPackageList();
    vector<Package>::iterator it;
    for ( it = list.begin() ; it != list.end() ; ++it )
    {
        QString text = "";

		// If the local server, only display installed packages
		if ( serverName == LOCAL_SERVER && !it->isInstalled() )
			continue;

        text += it->getPackageName();
        if ( it->isInstalled() )
        {
            text += " (installed)";

            // If a different version of package is available, postfix it with an *
            if ( it->getVersion() != it->getInstalledVersion() )
            	text += "*";
        }

        QCheckListItem *item = new QCheckListItem( packagesList, text, QCheckListItem::CheckBox );
        if ( !it->isPackageStoredLocally() )
            new QCheckListItem( item, QString( "Description - " ) + it->getDescription() );
        else
            new QCheckListItem( item, QString( "Filename - " ) + it->getFilename() );
        
        new QCheckListItem( item, QString( "V. Available - " ) + it->getVersion() );
        if ( it->getLocalPackage() )
        {
			if ( it->isInstalled() )
	            new QCheckListItem( item, QString( "V. Installed - " ) + it->getInstalledVersion() );
		}
        packagesList->insertItem( item );
    }

    // If the local server or the local ipkgs server disable the download button
    download->setText( "Download" );
    if ( serverName == LOCAL_SERVER )
        download->setEnabled( false );
    else if ( serverName == LOCAL_IPKGS )
    {
        download->setEnabled( true );
        download->setText( "Remove" );
    }
    else
        download->setEnabled( true );
}

void NetworkPackageManager :: updateServer()
{
    QString serverName = serversList->currentText();

    // Update the current server
    // Display dialog
    ProgressDlg *dlg = new ProgressDlg( this );
    QString status = "Updating package list for ";
    status += serverName;
    dlg->show();
    dlg->setText( status );

    // Disable buttons to stop silly people clicking lots on them :)

    // First, write out ipkg_conf file so that ipkg can use it
    dataMgr->writeOutIpkgConf();

    if ( serverName == LOCAL_SERVER )
        ;
    else if ( serverName == LOCAL_IPKGS )
        ;
    else
    {
        QString option = "update";
        QString dummy = "";
        Ipkg ipkg;
        connect( &ipkg, SIGNAL(outputText(const QString &)), this, SLOT(displayText(const QString &)));
        ipkg.setOption( option );
        
        ipkg.runIpkg( );
    }

    // Reload data
    dataMgr->reloadServerData( serversList->currentText() );
    serverSelected(-1);
    delete dlg;
}


void NetworkPackageManager :: downloadPackage()
{
    if ( download->text() == "Download" )
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

        QCheckListItem *item = (QCheckListItem *)packagesList->firstChild();
        ipkg.setOption( "download" );
        ipkg.setRuntimeDirectory( initDir );
        do
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

            item = (QCheckListItem *)item->nextSibling();
        } while ( item );
    }
    else if ( download->text() == "Remove" )
    {
        QCheckListItem *item = (QCheckListItem *)packagesList->firstChild();
        do
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
                QFile f( p->getFilename() );
                f.remove();
            }
            item = (QCheckListItem *)item->nextSibling();
        } while ( item );
    }

    dataMgr->reloadServerData( LOCAL_IPKGS );
    serverSelected( -1 );
}


void NetworkPackageManager :: applyChanges()
{
    // Disable buttons to stop silly people clicking lots on them :)

    // First, write out ipkg_conf file so that ipkg can use it
    dataMgr->writeOutIpkgConf();

    // Now for each selected item
    // deal with it

	vector<QString> workingPackages;
    QCheckListItem *item = (QCheckListItem *)packagesList->firstChild();
    do
    {
        if ( item->isOn() )
        {
            QString p = dealWithItem( item );
            workingPackages.push_back( p );
		}

        item = (QCheckListItem *)item->nextSibling();
    } while ( item );

    // do the stuff
    InstallDlgImpl dlg( workingPackages, dataMgr, this, "Install", true );
    dlg.showDlg();

    // Reload data
    dataMgr->reloadServerData( LOCAL_SERVER );
    dataMgr->reloadServerData( serversList->currentText() );
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
QString NetworkPackageManager :: dealWithItem( QCheckListItem *item )
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
    	return QString( "I" ) + name;
    else
    {
        if ( p->getVersion() == p->getInstalledVersion() )
        	return QString( "D" ) + name;
        else
        	return QString( "U" ) + name;
    }
}

void NetworkPackageManager :: displayText( const QString &t )
{
    cout << t << endl;
}
