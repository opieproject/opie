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
#include "searchmethoddlg.h"

#include <qdialog.h>
#include <qpe/config.h>
#include <qlayout.h>

#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qlineedit.h>

#include <opie/ofileselector.h>
#include <opie/ofiledialog.h>

SearchMethodDlg::SearchMethodDlg(QWidget *parent, const char *name, bool modal, QString itemname) : QDialog(parent, name, modal)
{
	if( !itemname )
		setCaption( tr( "New Searchmethod" ) );
	else 
	{
		setCaption( tr( "Change Searchmethod" ) );
		itemName = itemname;
		setupEntries(itemname);
	}

	QVBoxLayout *vbox_layout = new QVBoxLayout( this, 4,4,"vbox_layout" );
	QVBox *vbox = new QVBox( this );

	QHBox *hbox1 = new QHBox( vbox );
	QLabel *nameLabel = new QLabel( tr( "Name:" ) , hbox1 );
	nameLE = new QLineEdit( hbox1 );

	QLabel *dictLabel = new QLabel( tr( "Dictionary file" ), vbox );
	QHBox *hbox2 = new QHBox( vbox );
	dictFileLE = new QLineEdit( hbox2 );
	QPushButton *browseButton = new QPushButton( tr( "Browse" ) , hbox2 );
	connect( browseButton, SIGNAL( clicked() ), this, SLOT( slotBrowse() ) );

	QWidget *dummywidget = new QWidget( vbox );
	QLabel *lag1 = new QLabel( tr( "Language 1" ),dummywidget);
	QLabel *devider = new QLabel( tr( "Decollator" ),dummywidget);
	QLabel *lag2 = new QLabel( tr( "Language 2" ),dummywidget);
	lang1 = new QLineEdit( dummywidget );
	lang2 = new QLineEdit( dummywidget );
	trenner = new QLineEdit( dummywidget );
		trenner->setText( "::" );
	
	QGridLayout *grid = new QGridLayout( dummywidget, 2,3 );
	grid->addWidget( lag1, 0,0 );
	grid->addWidget( devider, 0,1 );
	grid->addWidget( lag2, 0,2 );
	grid->addWidget( lang1, 1,0 );
	grid->addWidget( trenner, 1,1 );
	grid->addWidget( lang2, 1,2 );
	
	vbox_layout->addWidget( vbox );

	showMaximized();
}

void SearchMethodDlg::setupEntries( QString item )
{
	Config cfg(  "odict" );
	cfg.setGroup( itemName );
	trenner->setText( "foooof" );
//X 	trenner->setText( cfg.readEntry( "Seperator" ) );
//X 	lang1->setText( cfg.readEntry( "Lang1" ) );
//X 	lang2->setText( cfg.readEntry( "Lang2" ) );
//X 	nameLE->setText( itemName );
//X 	dictFileLE->setText( cfg.readEntry( "file" ) );
}

void SearchMethodDlg::slotBrowse()
{
	itemName=OFileDialog::getOpenFileName( OFileSelector::EXTENDED,"/home/carsten" );
	dictFileLE->setText( itemName );
}

void SearchMethodDlg::saveItem()
{
	QString name = nameLE->text();
	Config cfg(  "odict" );
	cfg.setGroup( name );
	cfg.writeEntry( "Name", name );
	cfg.writeEntry( "Seperator", trenner->text() );
	cfg.writeEntry( "Lang1", lang1->text() );
	cfg.writeEntry( "Lang2", lang2->text() );
	cfg.writeEntry( "file", dictFileLE->text() );
}
