/***************************************************************************
                          settingsimpl.cpp  -  description
                             -------------------
    begin                : Thu Aug 29 2002
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
#include <algorithm>
using namespace std;

#include <qlistbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qcheckbox.h>

#ifdef QWS
#include <qpe/config.h>
#endif

#include "settingsimpl.h"

#include "global.h"

SettingsImpl :: SettingsImpl( DataManager *dataManager, QWidget * parent, const char* name, bool modal, WFlags fl )
    : SettingsBase( parent, name, modal, fl )
{
    dataMgr = dataManager;

    setupData();
    changed = false;
    newserver = false;
    newdestination = false;
}

SettingsImpl :: ~SettingsImpl()
{

}

bool SettingsImpl :: showDlg( int i )
{
	TabWidget->setCurrentPage( i );
	showMaximized();
	exec();

	if ( changed )
		dataMgr->writeOutIpkgConf();

	return changed;
}

void SettingsImpl :: setupData()
{
    // add servers
    vector<Server>::iterator it;
    for ( it = dataMgr->getServerList().begin() ; it != dataMgr->getServerList().end() ; ++it )
	{
        if ( it->getServerName() == LOCAL_SERVER || it->getServerName() == LOCAL_IPKGS )
            continue;

        servers->insertItem( it->getServerName() );
	}

    // add destinations
    vector<Destination>::iterator it2;
    for ( it2 = dataMgr->getDestinationList().begin() ; it2 != dataMgr->getDestinationList().end() ; ++it2 )
        destinations->insertItem( it2->getDestinationName() );

#ifdef QWS
    Config cfg( "aqpkg" );
    cfg.setGroup( "settings" );
    jumpTo->setChecked( cfg.readBoolEntry( "showJumpTo", "true" ) );
#else
    jumpTo->setChecked( true );
#endif
}

//------------------ Servers tab ----------------------

void SettingsImpl :: editServer( int sel )
{
	currentSelectedServer = sel;
    vector<Server>::iterator s = dataMgr->getServer( servers->currentText() );
    serverName = s->getServerName();
    servername->setText( s->getServerName() );
    serverurl->setText( s->getServerUrl() );
    active->setChecked( s->isServerActive() );
}

void SettingsImpl :: newServer()
{
	newserver = true;
	servername->setText( "" );
	serverurl->setText( "" );
	servername->setFocus();
    active->setChecked( true );
}

void SettingsImpl :: removeServer()
{
    changed = true;
    vector<Server>::iterator s = dataMgr->getServer( servers->currentText() );
    dataMgr->getServerList().erase( s );
    servers->removeItem( currentSelectedServer );
}

void SettingsImpl :: changeServerDetails()
{
	changed = true;

	QString newName = servername->text();
	if ( !newserver )
	{
		vector<Server>::iterator s = dataMgr->getServer( servers->currentText() );

		// Update url
		s->setServerUrl( serverurl->text() );
        s->setActive( active->isChecked() );


		// Check if server name has changed, if it has then we need to replace the key in the map
		if ( serverName != newName )
		{
			// Update server name
			s->setServerName( newName );

			// See if this server is the active server
//			if ( dataMgr->getActiveServer() == serverName )
//				dataMgr->setActiveServer( newName );

			// Update list box
			servers->changeItem( newName, currentSelectedServer );
		}
	}
	else
	{
        Server s( newName, serverurl->text() );
        dataMgr->getServerList().push_back( Server( newName, serverurl->text() ) );
        dataMgr->getServerList().end()->setActive( active->isChecked() );
		servers->insertItem( newName );
		servers->setCurrentItem( servers->count() );
		newserver = false;
	}
}

//------------------ Destinations tab ----------------------

void SettingsImpl :: editDestination( int sel )
{
	currentSelectedDestination = sel;
    vector<Destination>::iterator d = dataMgr->getDestination( destinations->currentText() );
    destinationName = d->getDestinationName();
    destinationname->setText( d->getDestinationName() );
    destinationurl->setText( d->getDestinationPath() );
    linkToRoot->setChecked( d->linkToRoot() );
}

void SettingsImpl :: newDestination()
{
	newdestination = true;
	destinationname->setText( "" );
	destinationurl->setText( "" );
	destinationname->setFocus();
    linkToRoot->setChecked( true );
}

void SettingsImpl :: removeDestination()
{
    changed = true;
    vector<Destination>::iterator d = dataMgr->getDestination( destinations->currentText() );
    dataMgr->getDestinationList().erase( d );
	destinations->removeItem( currentSelectedDestination );
}

void SettingsImpl :: changeDestinationDetails()
{
	changed = true;

#ifdef QWS
    Config cfg( "aqpkg" );
    cfg.setGroup( "destinations" );
#endif

    QString newName = destinationname->text();
	if ( !newdestination )
	{
		vector<Destination>::iterator d = dataMgr->getDestination( destinations->currentText() );

		// Update url
		d->setDestinationPath( destinationurl->text() );
        d->linkToRoot( linkToRoot->isChecked() );

		// Check if server name has changed, if it has then we need to replace the key in the map
		if ( destinationName != newName )
		{
			// Update server name
			d->setDestinationName( newName );

			// Update list box
			destinations->changeItem( newName, currentSelectedDestination );
		}
#ifdef QWS
        QString key = newName;
        key += "_linkToRoot";
        int val = d->linkToRoot();
        cfg.writeEntry( key, val );
#endif        
	}
	else
	{
		dataMgr->getDestinationList().push_back( Destination( newName, destinationurl->text() ) );
		destinations->insertItem( newName );
		destinations->setCurrentItem( destinations->count() );
		newdestination = false;

#ifdef QWS
        QString key = newName;
        key += "_linkToRoot";
        cfg.writeEntry( key, true );
#endif
	}
}

//------------------ General tab ----------------------

void SettingsImpl :: toggleJumpTo( bool val )
{
#ifdef QWS
    Config cfg( "aqpkg" );
    cfg.setGroup( "settings" );
    cfg.writeEntry( "showJumpTo", val );
#endif
}

