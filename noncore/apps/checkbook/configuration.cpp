/*
                             This file is part of the OPIE Project
               =.
             .=l.            Copyright (c)  2002 Dan Williams <drw@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "configuration.h"
#include "listedit.h"

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qwhatsthis.h>
#include <qlistview.h>
#include <qtabwidget.h>

Configuration::Configuration( QWidget *parent, Cfg &cfg )
	: QDialog( parent, 0, TRUE, WStyle_ContextHelp )
{
	setCaption( tr( "Configure Checkbook" ) );

    // Setup layout to make everything pretty
	QVBoxLayout *layout = new QVBoxLayout( this );
	layout->setMargin( 2 );
	layout->setSpacing( 4 );

	// Setup tabs for all info
	_mainWidget = new QTabWidget( this );
	layout->addWidget( _mainWidget );

    // Settings tab
    _mainWidget->addTab( initSettings(cfg), tr( "&Settings" ) );

    // Account Types tab
    ColumnDef *d;
    _listEditTypes=new ListEdit(_mainWidget, "TYPES" );
    d=new ColumnDef( tr("Type"), (ColumnDef::ColumnType)(ColumnDef::typeString | ColumnDef::typeUnique), tr("New Account Type"));
    _listEditTypes->addColumnDef( d );
    _listEditTypes->addData( cfg.getAccountTypes() );
    _mainWidget->addTab( _listEditTypes, tr( "&Account Types" ) );

    // Categories tab
    _listEditCategories=new ListEdit(_mainWidget, "CATEGORIES" );
    _listEditCategories->addColumnDef( new ColumnDef( tr("Category"), (ColumnDef::ColumnType)(ColumnDef::typeString | ColumnDef::typeUnique), tr("New Category")) );
    d=new ColumnDef(  tr("Type"), ColumnDef::typeList, tr("Expense") );
    d->addColumnValue( tr("Expense") );
    d->addColumnValue( tr("Income") );
    _listEditCategories->addColumnDef( d );
    QStringList lst=cfg.getCategories();
    _listEditCategories->addData( lst );
    _mainWidget->addTab( _listEditCategories, tr( "&Categories" ) );

    // Payees tab
    _listEditPayees=new ListEdit(_mainWidget, "PAYEES");
    _listEditPayees->addColumnDef( new ColumnDef( tr("Payee"), (ColumnDef::ColumnType)(ColumnDef::typeString | ColumnDef::typeUnique), tr("New Payee")) );
    _listEditPayees->addData( cfg.getPayees() );
    _mainWidget->addTab( _listEditPayees, tr("&Payees") );
}

Configuration::~Configuration()
{
}

// ---- initSettings ----------------------------------------------------------
QWidget *Configuration::initSettings(Cfg &cfg)
{
    QWidget *control = new QWidget( _mainWidget );

    QFontMetrics fm = fontMetrics();
	int fh = fm.height();

    QVBoxLayout *vb = new QVBoxLayout( control );

	QScrollView *sv = new QScrollView( control );
	vb->addWidget( sv, 0, 0 );
	sv->setResizePolicy( QScrollView::AutoOneFit );
	sv->setFrameStyle( QFrame::NoFrame );

	QWidget *container = new QWidget( sv->viewport() );
	sv->addChild( container );

	QGridLayout *layout = new QGridLayout( container );
	layout->setSpacing( 4 );
	layout->setMargin( 4 );

	QLabel *label = new QLabel( tr( "Enter currency symbol:" ), container );
	QWhatsThis::add( label, tr( "Enter your local currency symbol here." ) );
	label->setMaximumHeight( fh + 3 );
	layout->addWidget( label, 0, 0 );

	symbolEdit = new QLineEdit( cfg.getCurrencySymbol(), container );
	QWhatsThis::add( symbolEdit, tr( "Enter your local currency symbol here." ) );
	symbolEdit->setMaximumHeight( fh + 5 );
	symbolEdit->setFocus();
    layout->addWidget( symbolEdit, 0, 1 );

	lockCB = new QCheckBox( tr( "Show whether checkbook is password\nprotected" ), container );
	QWhatsThis::add( lockCB, tr( "Click here to select whether or not the main window will display that the checkbook is protected with a password." ) );
	lockCB->setChecked( cfg.getShowLocks() );
	layout->addMultiCellWidget( lockCB, 1, 1, 0, 1 );

	balCB = new QCheckBox( tr( "Show checkbook balances" ), container );
	QWhatsThis::add( balCB, tr( "Click here to select whether or not the main window will display the current balance for each checkbook." ) );
	balCB->setMaximumHeight( fh + 5 );
	balCB->setChecked( cfg.getShowBalances() );
	layout->addMultiCellWidget( balCB, 2, 2, 0, 1 );

    openLastBookCB = new QCheckBox( tr("Open last checkbook" ), container );
    QWhatsThis::add( openLastBookCB, tr("Click here to select whether the last open checkbook will be opened at startup.") );
    openLastBookCB->setMaximumHeight(fh+5);
    openLastBookCB->setChecked( cfg.isOpenLastBook() );
    layout->addMultiCellWidget( openLastBookCB, 3, 3, 0, 1 );

    lastTabCB = new QCheckBox( tr("Show last checkbook tab" ), container );
    QWhatsThis::add( lastTabCB, tr("Click here to select whether the last tab in a checkbook should be displayed.") );
    lastTabCB->setMaximumHeight(fh+5);
    lastTabCB->setChecked( cfg.isShowLastTab() );
    layout->addMultiCellWidget( lastTabCB, 4, 4, 0, 1 );

    savePayees = new QCheckBox( tr("Save new description as payee"), container );
    QWhatsThis::add( savePayees, tr("Click here to save new descriptions in the list of payess.") );
    savePayees->setMaximumHeight(fh+5);
    savePayees->setChecked( cfg.getSavePayees() );
    layout->addMultiCellWidget( savePayees, 5, 5, 0, 1 );

    return(control);
}

// --- saveConfig -------------------------------------------------------------
void Configuration::saveConfig(Cfg &cfg)
{
    // Settings
    cfg.setCurrencySymbol( symbolEdit->text() );
    cfg.setShowLocks( lockCB->isChecked() );
    cfg.setShowBalances( balCB->isChecked() );
    cfg.setOpenLastBook( openLastBookCB->isChecked() );
    cfg.setShowLastTab( lastTabCB->isChecked() );
    cfg.setSavePayees( savePayees->isChecked() );

    // Typelist
    _listEditTypes->storeInList( cfg.getAccountTypes() );

    // Category list
    QStringList lst;
    _listEditCategories->storeInList( lst );
    cfg.setCategories( lst );

    // Payees
    _listEditPayees->storeInList( cfg.getPayees() );
}
