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
#include "configdlg.h"
#include "searchmethoddlg.h"

#include <qpe/qpeapplication.h>
#include <qpe/config.h>

#include <qdialog.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qstringlist.h>

ConfigDlg::ConfigDlg(QWidget *parent, const char *name, bool modal) : QDialog(parent, name, modal)
{
	setCaption( tr( "Options" ) );
	QVBoxLayout *vbox_layout = new QVBoxLayout( this );
	search_tab = new QWidget( this , "search_tab" );
	QVBoxLayout *vbox_layout_searchtab = new QVBoxLayout( search_tab, 4 , 4 ,"blah" );

	QHBox *hbox = new QHBox( search_tab );
	list = new QListView( hbox );
	list->addColumn( tr( "Searchmethod" ) );
	loadSearchMethodNames();
	
	QVBox *vbox = new QVBox( hbox );
	new_button = new QPushButton( tr( "New" ) , vbox );
	change_button = new QPushButton( tr( "Change" ) , vbox );
	delete_button = new QPushButton( tr( "Delete" ) , vbox );
	connect( new_button, SIGNAL( clicked() ), this, SLOT( slotNewMethod() ) );
	connect( change_button, SIGNAL( clicked() ), this, SLOT( slotChangeMethod() ));
	connect( delete_button, SIGNAL( clicked() ), this, SLOT( slotDeleteMethod() ));
	
	vbox_layout_searchtab->addWidget( hbox );

	vbox_layout->addWidget( search_tab );

	QPEApplication::execDialog( this );
}

void ConfigDlg::slotNewMethod()
{
	SearchMethodDlg dlg( this, "SearchMethodDlg", true );
	if ( dlg.exec() == QDialog::Accepted )
	{
		dlg.saveItem();
		QListViewItem *item = new QListViewItem( list );
		item->setText( 0 , dlg.nameLE->text() );
	}
}

void ConfigDlg::slotChangeMethod()
{
	if ( list->selectedItem() )
	{
		SearchMethodDlg dlg( this, "SearchMethodDlg", true, list->selectedItem()->text( 0 ) );
		if ( dlg.exec() == QDialog::Accepted )
		{
			dlg.saveItem();
			QListViewItem *item = list->selectedItem();
			item->setText( 0 , dlg.nameLE->text() );
		}
	}
}

void ConfigDlg::slotDeleteMethod()
{
	if ( list->selectedItem() )
	{
		Config cfg ( "odict" );
		cfg.setGroup( "Method_"+list->selectedItem()->text(0) );
		cfg.clearGroup();
		//FIXME: this only removes the entries but not the group itself
		
		list->takeItem( list->selectedItem() );
	}
}

void ConfigDlg::loadSearchMethodNames()
{
	Config cfg(  "odict" );
	QStringList groupListCfg = cfg.groupList().grep( "Method_" );
	for ( QStringList::Iterator it = groupListCfg.begin() ; it != groupListCfg.end() ; ++it )
	{
		QListViewItem *item = new QListViewItem( list );
		cfg.setGroup( *it );
		item->setText( 0 , cfg.readEntry( "Name" ) );
	}
}
