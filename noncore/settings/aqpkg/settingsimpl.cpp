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

#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>

#include <opie/otabwidget.h>

#ifdef QWS
#include <qpe/config.h>
#include <qpe/resource.h>
#endif

#include "settingsimpl.h"

#include "global.h"

SettingsImpl :: SettingsImpl( DataManager *dataManager, QWidget * parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    setCaption( tr( "Configuration" ) );
    
    // Setup layout to make everything pretty
    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->setMargin( 2 );
    layout->setSpacing( 4 );

    // Setup tabs for all info
    OTabWidget *tabwidget = new OTabWidget( this );
    layout->addWidget( tabwidget );

    tabwidget->addTab( initServerTab(), "aqpkg/servertab", tr( "Servers" ) );
    tabwidget->addTab( initDestinationTab(), "aqpkg/desttab", tr( "Destinations" ) );
    tabwidget->addTab( initProxyTab(), "aqpkg/proxytab", tr( "Proxies" ) );
    tabwidget->setCurrentTab( tr( "Servers" ) );
    
    dataMgr = dataManager;
    setupData();
    changed = false;
    newserver = false;
    newdestination = false;
}

SettingsImpl :: ~SettingsImpl()
{

}

bool SettingsImpl :: showDlg()
{
	showMaximized();
	exec();

	if ( changed )
		dataMgr->writeOutIpkgConf();

	return changed;
}

QWidget *SettingsImpl :: initServerTab()
{
    QWidget *control = new QWidget( this );

    QVBoxLayout *vb = new QVBoxLayout( control );

    QScrollView *sv = new QScrollView( control );
    vb->addWidget( sv, 0, 0 );
    sv->setResizePolicy( QScrollView::AutoOneFit );
    sv->setFrameStyle( QFrame::NoFrame );

    QWidget *container = new QWidget( sv->viewport() );
    sv->addChild( container );

    QGridLayout *layout = new QGridLayout( container );
    layout->setSpacing( 2 );
    layout->setMargin( 4 );

    servers = new QListBox( container );
    servers->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    connect( servers, SIGNAL( highlighted( int ) ), this, SLOT( editServer( int ) ) );
    layout->addMultiCellWidget( servers, 0, 0, 0, 1 );

    QPushButton *btn = new QPushButton( Resource::loadPixmap( "new" ), tr( "New" ), container );
    connect( btn, SIGNAL( clicked() ), this, SLOT( newServer() ) );
    layout->addWidget( btn, 1, 0 );
    
    btn = new QPushButton( Resource::loadPixmap( "trash" ), tr( "Delete" ), container );
    connect( btn, SIGNAL( clicked() ), this, SLOT( removeServer() ) );
    layout->addWidget( btn, 1, 1 );
    
    QGroupBox *grpbox = new QGroupBox( 0, Qt::Vertical, tr( "Server" ), container );
    grpbox->layout()->setSpacing( 2 );
    grpbox->layout()->setMargin( 4 );
    layout->addMultiCellWidget( grpbox, 2, 2, 0, 1 );

    QGridLayout *grplayout = new QGridLayout( grpbox->layout() );
    
    QLabel *label = new QLabel( tr( "Name:" ), grpbox );
    grplayout->addWidget( label, 0, 0 );
    servername = new QLineEdit( grpbox );
    grplayout->addWidget( servername, 0, 1 );

    label = new QLabel( tr( "Address:" ), grpbox );
    grplayout->addWidget( label, 1, 0 );
    serverurl = new QLineEdit( grpbox );
    grplayout->addWidget( serverurl, 1, 1 );

    active = new QCheckBox( tr( "Active Server" ), grpbox );
    grplayout->addMultiCellWidget( active, 2, 2, 0, 1 );
    
    btn = new QPushButton( Resource::loadPixmap( "edit" ), tr( "Update" ), grpbox );
    connect( btn, SIGNAL( clicked() ), this, SLOT( changeServerDetails() ) );
    grplayout->addMultiCellWidget( btn, 3, 3, 0, 1 );
    
    return control;
}

QWidget *SettingsImpl :: initDestinationTab()
{
    QWidget *control = new QWidget( this );

    QVBoxLayout *vb = new QVBoxLayout( control );

    QScrollView *sv = new QScrollView( control );
    vb->addWidget( sv, 0, 0 );
    sv->setResizePolicy( QScrollView::AutoOneFit );
    sv->setFrameStyle( QFrame::NoFrame );

    QWidget *container = new QWidget( sv->viewport() );
    sv->addChild( container );

    QGridLayout *layout = new QGridLayout( container );
    layout->setSpacing( 2 );
    layout->setMargin( 4 );

    destinations = new QListBox( container );
    destinations->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    connect( destinations, SIGNAL( highlighted( int ) ), this, SLOT( editDestination( int ) ) );
    layout->addMultiCellWidget( destinations, 0, 0, 0, 1 );

    QPushButton *btn = new QPushButton( Resource::loadPixmap( "new" ), tr( "New" ), container );
    connect( btn, SIGNAL( clicked() ), this, SLOT( newDestination() ) );
    layout->addWidget( btn, 1, 0 );
    
    btn = new QPushButton( Resource::loadPixmap( "trash" ), tr( "Delete" ), container );
    connect( btn, SIGNAL( clicked() ), this, SLOT( removeDestination() ) );
    layout->addWidget( btn, 1, 1 );
    
    QGroupBox *grpbox = new QGroupBox( 0, Qt::Vertical, tr( "Destination" ), container );
    grpbox->layout()->setSpacing( 2 );
    grpbox->layout()->setMargin( 4 );
    layout->addMultiCellWidget( grpbox, 2, 2, 0, 1 );

    QGridLayout *grplayout = new QGridLayout( grpbox->layout() );
    
    QLabel *label = new QLabel( tr( "Name:" ), grpbox );
    grplayout->addWidget( label, 0, 0 );
    destinationname = new QLineEdit( grpbox );
    grplayout->addWidget( destinationname, 0, 1 );

    label = new QLabel( tr( "Location:" ), grpbox );
    grplayout->addWidget( label, 1, 0 );
    destinationurl = new QLineEdit( grpbox );
    grplayout->addWidget( destinationurl, 1, 1 );

    linkToRoot = new QCheckBox( tr( "Link to root" ), grpbox );
    grplayout->addMultiCellWidget( linkToRoot, 2, 2, 0, 1 );
    
    btn = new QPushButton( Resource::loadPixmap( "edit" ), tr( "Update" ), grpbox );
    connect( btn, SIGNAL( clicked() ), this, SLOT( changeDestinationDetails() ) );
    grplayout->addMultiCellWidget( btn, 3, 3, 0, 1 );
    
    return control;
}

QWidget *SettingsImpl :: initProxyTab()
{
    QWidget *control = new QWidget( this );

    QVBoxLayout *vb = new QVBoxLayout( control );

    QScrollView *sv = new QScrollView( control );
    vb->addWidget( sv, 0, 0 );
    sv->setResizePolicy( QScrollView::AutoOneFit );
    sv->setFrameStyle( QFrame::NoFrame );

    QWidget *container = new QWidget( sv->viewport() );
    sv->addChild( container );

    QGridLayout *layout = new QGridLayout( container );
    layout->setSpacing( 2 );
    layout->setMargin( 4 );

    QGroupBox *grpbox = new QGroupBox( 0, Qt::Vertical, tr( "HTTP Proxy" ), container );
    grpbox->layout()->setSpacing( 2 );
    grpbox->layout()->setMargin( 4 );
    layout->addMultiCellWidget( grpbox, 0, 0, 0, 1 );
    QVBoxLayout *grplayout = new QVBoxLayout( grpbox->layout() );
    txtHttpProxy = new QLineEdit( grpbox );
    grplayout->addWidget( txtHttpProxy );
    chkHttpProxyEnabled = new QCheckBox( tr( "Enabled" ), grpbox );
    grplayout->addWidget( chkHttpProxyEnabled );
    
    grpbox = new QGroupBox( 0, Qt::Vertical, tr( "FTP Proxy" ), container );
    grpbox->layout()->setSpacing( 2 );
    grpbox->layout()->setMargin( 4 );
    layout->addMultiCellWidget( grpbox, 1, 1, 0, 1 );
    grplayout = new QVBoxLayout( grpbox->layout() );
    txtFtpProxy = new QLineEdit( grpbox );
    grplayout->addWidget( txtFtpProxy );
    chkFtpProxyEnabled = new QCheckBox( tr( "Enabled" ), grpbox );
    grplayout->addWidget( chkFtpProxyEnabled );
    
    QLabel *label = new QLabel( tr( "Username:" ), container );
    layout->addWidget( label, 2, 0 );
    txtUsername = new QLineEdit( container );
    layout->addWidget( txtUsername, 2, 1 );

    label = new QLabel( tr( "Password:" ), container );
    layout->addWidget( label, 3, 0 );
    txtPassword = new QLineEdit( container );
    layout->addWidget( txtPassword, 3, 1 );

    QPushButton *btn = new QPushButton( Resource::loadPixmap( "edit" ), tr( "Update" ), container );
    connect( btn, SIGNAL( clicked() ), this, SLOT( proxyApplyChanges() ) );
    layout->addMultiCellWidget( btn, 4, 4, 0, 1 );
    
    return control;
}

void SettingsImpl :: setupData()
{
    // add servers
    QString serverName;
    QListIterator<Server> it( dataMgr->getServerList() );
    for ( ; it.current(); ++it )
	{
        serverName = it.current()->getServerName();
        if ( serverName == LOCAL_SERVER || serverName == LOCAL_IPKGS )
            continue;

        servers->insertItem( serverName );
	}


    // add destinations
    QListIterator<Destination> it2( dataMgr->getDestinationList() );
    for ( ; it2.current(); ++it2 )
        destinations->insertItem( it2.current()->getDestinationName() );
    
    // setup proxy tab
    txtHttpProxy->setText( dataMgr->getHttpProxy() );
    txtFtpProxy->setText( dataMgr->getFtpProxy() );
    txtUsername->setText( dataMgr->getProxyUsername() );
    txtPassword->setText( dataMgr->getProxyPassword() );
    chkHttpProxyEnabled->setChecked( dataMgr->getHttpProxyEnabled() );
    chkFtpProxyEnabled->setChecked( dataMgr->getFtpProxyEnabled() );
}

//------------------ Servers tab ----------------------

void SettingsImpl :: editServer( int sel )
{
    currentSelectedServer = sel;
    Server *s = dataMgr->getServer( servers->currentText() );
    if ( s )
    {
        serverName = s->getServerName();
        servername->setText( s->getServerName() );
        serverurl->setText( s->getServerUrl() );
        active->setChecked( s->isServerActive() );
    }
    else
    {
        serverName = "";
        servername->setText( "" );
        serverurl->setText( "" );
        active->setChecked( false );
    }
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
    Server *s = dataMgr->getServer( servers->currentText() );
    if ( s )
    {
        dataMgr->getServerList().removeRef( s );
        servers->removeItem( currentSelectedServer );
    }
}

void SettingsImpl :: changeServerDetails()
{
	changed = true;

	QString newName = servername->text();
	
	// Convert any spaces to underscores
	char *tmpStr = new char[newName.length() + 1];
	for ( unsigned int i = 0 ; i < newName.length() ; ++i )
	{
		if ( newName[i] == ' ' )
			tmpStr[i] = '_';
		else
			tmpStr[i] = newName[i].latin1();
	}
	tmpStr[newName.length()] = '\0';
	
	newName = tmpStr;
	delete tmpStr;
	
	if ( !newserver )
	{
		Server *s = dataMgr->getServer( servers->currentText() );
		if ( s )
		{
			// Update url
			s->setServerUrl( serverurl->text() );
			s->setActive( active->isChecked() );

			// Check if server name has changed, if it has then we need to replace the key in the map
			if ( serverName != newName )
			{
				// Update server name
				s->setServerName( newName );
			}
		
			// Update list box
			servers->changeItem( newName, currentSelectedServer );
		}
	}
	else
	{
        Server s( newName, serverurl->text() );
        dataMgr->getServerList().append( new Server( newName, serverurl->text() ) );
        dataMgr->getServerList().last()->setActive( active->isChecked() );
		servers->insertItem( newName );
		servers->setCurrentItem( servers->count() );
		newserver = false;
	}
}

//------------------ Destinations tab ----------------------

void SettingsImpl :: editDestination( int sel )
{
    currentSelectedDestination = sel;
    Destination *d = dataMgr->getDestination( destinations->currentText() );
    if ( d )
    {
        destinationName = d->getDestinationName();
        destinationname->setText( d->getDestinationName() );
        destinationurl->setText( d->getDestinationPath() );
        linkToRoot->setChecked( d->linkToRoot() );
    }
    else
    {
        destinationName = "";
        destinationname->setText( "" );
        destinationurl->setText( "" );
        linkToRoot->setChecked( false );
    }
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
    Destination *d = dataMgr->getDestination( destinations->currentText() );
    if ( d )
    {
        dataMgr->getDestinationList().removeRef( d );
        destinations->removeItem( currentSelectedDestination );
    }
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
		Destination *d = dataMgr->getDestination( destinations->currentText() );
		if ( d )
		{
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
	}
	else
	{
		dataMgr->getDestinationList().append( new Destination( newName, destinationurl->text() ) );
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

//------------------ Proxy tab ----------------------
void SettingsImpl :: proxyApplyChanges()
{
    changed = true;
    dataMgr->setHttpProxy( txtHttpProxy->text() );
    dataMgr->setFtpProxy( txtFtpProxy->text() );
    dataMgr->setProxyUsername( txtUsername->text() );
    dataMgr->setProxyPassword( txtPassword->text() );

    dataMgr->setHttpProxyEnabled( chkHttpProxyEnabled->isChecked() );
    dataMgr->setFtpProxyEnabled( chkFtpProxyEnabled->isChecked() );
}
