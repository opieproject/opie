/***************************************************************************
                          installdlgimpl.cpp  -  description
                             -------------------
    begin                : Mon Aug 26 2002
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

#ifdef QWS
#include <qpe/config.h>
#endif

#include <qmultilineedit.h>
#include <qdialog.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>


#include "datamgr.h"
#include "instoptionsimpl.h"
#include "destination.h"
#include "installdlgimpl.h"
#include "global.h"

InstallDlgImpl::InstallDlgImpl( vector<QString> &packageList, DataManager *dataManager, QWidget * parent, const char* name, bool modal, WFlags fl )
    : InstallDlg( parent, name, modal, fl )
{
    upgradePackages = false;
    dataMgr = dataManager;
    vector<Destination>::iterator dit;

    QString defaultDest = "root";
#ifdef QWS
    Config cfg( "aqpkg" );
    cfg.setGroup( "settings" );
    defaultDest = cfg.readEntry( "dest", "root" );

    // Grab flags - Turn MAKE_LINKS on by default (if no flags found)
    flags = cfg.readNumEntry( "installFlags", MAKE_LINKS );
#else
	flags = 0;
#endif

    // Output text is read only
    output->setReadOnly( true );
	QFont f( "helvetica" );
	f.setPointSize( 10 );
	output->setFont( f );


    // setup destination data
    int defIndex = 0;
    int i;
    for ( i = 0 , dit = dataMgr->getDestinationList().begin() ; dit != dataMgr->getDestinationList().end() ; ++dit, ++i )
    {
        destination->insertItem( dit->getDestinationName() );
        if ( dit->getDestinationName() == defaultDest )
            defIndex = i;
    }

    destination->setCurrentItem( defIndex );

	vector<QString>::iterator it;
	// setup package data
	QString remove = "Remove\n";
	QString install = "\nInstall\n";
	QString upgrade = "\nUpgrade\n";
	for ( it = packageList.begin() ; it != packageList.end() ; ++it )
	{
		QString name = *it;
		if ( name.startsWith( "I" ) )
		{
			installList.push_back( name.mid(1) );
			install += "   " + name.mid(1) + "\n";
		}
		else if ( name.startsWith( "D" ) )
		{
			removeList.push_back( name.mid(1) );
			remove += "   " + name.mid(1) + "\n";
		}
		else if ( name.startsWith( "U" ) )
		{
			updateList.push_back( name.mid(1) );
			upgrade += "   " + name.mid(1) + "\n";
		}
	}

	output->setText( remove + install + upgrade );

    connect( &ipkg, SIGNAL(outputText(const QString &)), this, SLOT(displayText(const QString &)));
}

InstallDlgImpl::InstallDlgImpl( QWidget *parent, const char *name, bool modal, WFlags fl )
    : InstallDlg( parent, name, modal, fl )
{
    upgradePackages = true;
    output->setText( "Upgrading installed packages" );
    connect( &ipkg, SIGNAL(outputText(const QString &)), this, SLOT(displayText(const QString &)));
}


InstallDlgImpl::~InstallDlgImpl()
{
}

bool InstallDlgImpl :: showDlg()
{
	showMaximized();
	bool ret = exec();

	return ret;
}

void InstallDlgImpl :: optionsSelected()
{
	InstallOptionsDlgImpl opt( flags, this, "Option", true );
	opt.exec();

	// set options selected from dialog
    flags = 0;
    if ( opt.forceDepends->isChecked() )
        flags |= FORCE_DEPENDS;
    if ( opt.forceReinstall->isChecked() )
        flags |= FORCE_REINSTALL;
    if ( opt.forceRemove->isChecked() )
        flags |= FORCE_REMOVE;
    if ( opt.forceOverwrite->isChecked() )
        flags |= FORCE_OVERWRITE;
    if ( opt.makeLinks->isChecked() )
        flags |= MAKE_LINKS;

#ifdef QWS
    Config cfg( "aqpkg" );
    cfg.setGroup( "settings" );
    cfg.writeEntry( "installFlags", flags );
#endif
}

void InstallDlgImpl :: installSelected()
{
	if ( btnInstall->text() == "Close" )
	{
		done( 1 );
		return;
	}

	btnInstall->setEnabled( false );

    if ( upgradePackages )
    {
        output->setText( "" );
        
        Ipkg ipkg;
        connect( &ipkg, SIGNAL(outputText(const QString &)), this, SLOT(displayText(const QString &)));
        ipkg.setOption( "upgrade" );
        ipkg.runIpkg();
    }
    else
    {
        output->setText( "" );
        Destination *d = dataMgr->getDestination( destination->currentText() );
        QString dest = d->getDestinationName();
        QString destDir = d->getDestinationPath();
    
#ifdef QWS
        // Save settings
        Config cfg( "aqpkg" );
        cfg.setGroup( "settings" );
        cfg.writeEntry( "dest", dest );
#endif

        // First run through the remove list, then the install list then the upgrade list
    	vector<QString>::iterator it;
        ipkg.setOption( "remove" );
        ipkg.setDestination( dest );
        ipkg.setDestinationDir( destDir );
        ipkg.setFlags( flags );
        for ( it = removeList.begin() ; it != removeList.end() ; ++it )
        {
            ipkg.setPackage( *it );
            ipkg.runIpkg();
        }

        ipkg.setOption( "install" );
        for ( it = installList.begin() ; it != installList.end() ; ++it )
        {
            ipkg.setPackage( *it );
            ipkg.runIpkg();
        }

        flags |= FORCE_REINSTALL;
        ipkg.setFlags( flags );
        for ( it = updateList.begin() ; it != updateList.end() ; ++it )
        {
            ipkg.setPackage( *it );
            ipkg.runIpkg();
        }
    }

	btnInstall->setEnabled( true );
    btnInstall->setText( tr( "Close" ) );
}

void InstallDlgImpl :: displayText(const QString &text )
{
    QString t = output->text() + "\n" + text;
    output->setText( t );
    output->setCursorPosition( output->numLines(), 0 );
}
