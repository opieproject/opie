/***************************************************************************
   application:             : ODict

   begin                    : December 2002
   copyright                : ( C ) 2002, 2003 by Carsten Niehaus
   email                    : cniehaus@handhelds.org
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/
#include "odict.h"
#include "configdlg.h"
#include "dingwidget.h"

#include <qmenubar.h>
#include <qmessagebox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qaction.h>
#include <qtextbrowser.h>
#include <qcombobox.h>

#include <qpe/resource.h>
#include <qpe/config.h>

ODict::ODict(QWidget* parent, const char* name, WFlags fl ) : QMainWindow(parent,  name, fl )
{
	activated_name = QString::null;

	vbox = new QVBox( this );
	setCaption( tr( "Opie-Dictionary" ) );
	setupMenus();

	QHBox *hbox = new QHBox( vbox );
	QLabel* query_label = new QLabel( tr( "Query:" ) , hbox );
	query_label->show();
	query_le = new QLineEdit( hbox );
	query_co = new QComboBox( hbox );
	connect( query_co , SIGNAL( activated(const QString&) ), this, SLOT( slotMethodChanged(const QString&) ) );
	ok_button = new QPushButton( tr( "&Ok" ), hbox );
	connect( ok_button, SIGNAL( released() ), this, SLOT( slotStartQuery() ) );

	top_name = new QLabel( vbox );
	top_name->setAlignment( AlignHCenter );
	browser_top = new QTextBrowser( vbox );
	bottom_name = new QLabel( vbox );
	bottom_name->setAlignment( AlignHCenter );
	browser_bottom = new QTextBrowser( vbox );

	ding = new DingWidget();

	loadConfig();
	setCentralWidget( vbox );
}

void ODict::loadConfig()
{
	/*
	 * the name of the last used dictionary
	 */
	QString lastname;

	Config cfg ( "odict" );
	cfg.setGroup( "generalsettings" );
	casesens = cfg.readEntry( "casesens" ).toInt();

	QString lastDict = cfg.readEntry( "lastdict" );
	int i = 0, e = 0;

	QStringList groupListCfg = cfg.groupList().grep( "Method_" );
	query_co->clear();
	for ( QStringList::Iterator it = groupListCfg.begin() ; it != groupListCfg.end() ; ++it )
	{
		QString name;
		cfg.setGroup( *it );
		name = cfg.readEntry( "Name" );
		if ( name != QString::null ) {
			query_co->insertItem( name );
		}

		/*
		 * this check is to look up what dictionary has been used the
		 * last time
		 */
		if ( lastDict == name )
		{
			e = i;
			lastname = name;
		}
		i++;
	}
	/*
	 * now set the two names of the dictionary and the correct QComboBox-Entry
	 */

	lookupLanguageNames( lastname );
	ding->loadDict( lastname );
	ding->loadValues();

	query_co->setCurrentItem( e );
	top_name->setText( top_name_content );
	bottom_name->setText( bottom_name_content );
}

void ODict::lookupLanguageNames( QString dictname )
{
	Config cfg ( "odict" );
	cfg.setGroup( "Method_"+dictname );
	top_name_content = cfg.readEntry( "Lang1" );
	bottom_name_content = cfg.readEntry( "Lang2" );
}

void ODict::saveConfig()
{
	Config cfg ( "odict" );
	cfg.setGroup( "generalsettings" );
	cfg.writeEntry( "casesens" , casesens );
	cfg.writeEntry( "lastdict" , query_co->currentText() );
}

void ODict::slotStartQuery()
{
	QString querystring = query_le->text();
	if ( !querystring.isEmpty() )
	{
		/*
		 * if the user has not yet defined a dictionary
		 */
		if ( !query_co->currentText() )
		{
			switch (  QMessageBox::information(  this, tr( "OPIE-Dictionary" ),
						tr( "No dictionary defined" ),
						tr( "&Define one" ),
						tr( "&Cancel" ),
						0,      // Define a dict
						1 ) )   // Cancel choosen
			{
				case 0:
					slotSettings();
					break;
				case 1: // stop here
					return;
			}
		}

		/*
		 * ok, the user has defined a dict
		 */
		ding->setCaseSensitive( casesens );

		BroswerContent test = ding->setText( querystring );

		browser_top->setText( test.top );
		browser_bottom->setText( test.bottom );
	}
}

void ODict::slotSettings()
{
	ConfigDlg dlg( this, "Config" , true);
	if ( dlg.exec() == QDialog::Accepted )
		saveConfig();
}

void ODict::slotSetParameter( int count )
{
 	if ( count == 0 )
	{
		if ( casesens )
			casesens = false;
		else
			casesens = true;
	}

	saveConfig();
}

void ODict::slotMethodChanged( const QString& methodnumber )
{
	activated_name =  methodnumber;

	if ( activated_name != ding->loadedDict() )
	{
		ding->loadDict(activated_name);

		lookupLanguageNames( activated_name );
		top_name->setText( top_name_content );
		bottom_name->setText( bottom_name_content );
	}
}

void ODict::setupMenus()
{
	menu = new QMenuBar( this );

	settings = new QPopupMenu( menu );
	setting_a = new QAction(tr( "Configuration" ), Resource::loadPixmap( "new" ), QString::null, 0, this, 0 );
	connect( setting_a, SIGNAL( activated() ), this, SLOT( slotSettings() ) );
	setting_a->addTo( settings );
	setting_b = new QAction(tr(  "Searchmethods" ), Resource::loadPixmap( "edit" ), QString::null, 0, this, 0 );

	parameter = new QPopupMenu( menu );
	connect(  parameter, SIGNAL( activated(int) ), this, SLOT( slotSetParameter(int) ) );
	parameter->insertItem( tr( "C&ase sensitive" ), 0 ,0 );
	parameter->insertSeparator();

	menu->insertItem( tr( "Settings" ) , settings );
	menu->insertItem( tr( "Parameter" ) , parameter );
}
