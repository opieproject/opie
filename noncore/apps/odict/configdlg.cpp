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

#include <qdialog.h>
#include <qpe/config.h>
#include <qlayout.h>

#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qpushbutton.h>

#include <opie/otabwidget.h>

ConfigDlg::ConfigDlg(QWidget *parent, const char *name, bool modal) : QDialog(parent, name, modal)
{
	setCaption( tr( "Options" ) );
	QVBoxLayout *vbox_layout = new QVBoxLayout( this );
	tab = new OTabWidget( this, "OTabWidget_tab", OTabWidget::Global, OTabWidget::Bottom );
	vbox_layout->addWidget( tab );

	/*general settings*/
	settings_tab = new QWidget( tab , "settings_tab" );

	/*searchmethods*/
	search_tab = new QWidget( tab , "search_tab" );
	QVBoxLayout *vbox_layout_searchtab = new QVBoxLayout( search_tab, 4 , 4 ,"blah" );
	
	QHBox *hbox = new QHBox( search_tab );
	list = new QListView( hbox );
	list->addColumn( tr( "Searchmethod" ) );
	QListViewItem *item = new QListViewItem( list );
	item->setText( 0, "foofooofoofoof" );
	
	QVBox *vbox = new QVBox( hbox );
	new_button = new QPushButton( "New" , vbox );
	change_button = new QPushButton( "Change" , vbox );
	delete_button = new QPushButton( "Delete" , vbox );
	connect( new_button, SIGNAL( clicked() ), this, SLOT( slotNewMethod() ) );
	connect( change_button, SIGNAL( clicked() ), this, SLOT( slotChangeMethod() ));
	connect( delete_button, SIGNAL( clicked() ), this, SLOT( slotDeleteMethod() ));
	
	vbox_layout_searchtab->addWidget( hbox );

	/*add the tabs and maximize*/
	tab->addTab( settings_tab, "pass", tr( "General Settings" ) );
	tab->addTab( search_tab, "zoom", tr( "Searchmethods" ) );
	showMaximized();
}

void ConfigDlg::writeEntries()
{
	qDebug( "richtig beendet" );
}
		
void ConfigDlg::slotNewMethod()
{
	SearchMethodDlg dlg( this, "SearchMethodDlg", true );
	if ( dlg.exec() == QDialog::Accepted )
	{
		//dlg.saveItem();
		QListViewItem *item = new QListViewItem( list );
		item->setText( 0 , dlg.itemName );
	}
	else qDebug( "SearchMethodDlg abgebrochen" );
}

void ConfigDlg::slotChangeMethod(){}

void ConfigDlg::slotDeleteMethod()
{
	if ( list->selectedItem() )
		list->takeItem( list->selectedItem() );
	else qDebug("no item selected"); 
}
