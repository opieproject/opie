/*
                             This file is part of the OPIE Project
               =.
             .=l.            Copyright (c)  2002 Dan Williams <williamsdr@acm.org>
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

#include "checkbook.h"
#include "transaction.h"
#include "graph.h"
#include "graphinfo.h"

#include <opie/otabwidget.h>
#include <qpe/config.h>
#include <qpe/qpeapplication.h>
#include <qpe/qpemessagebox.h>
#include <qpe/resource.h>

#include <qcombobox.h>
#include <qfile.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qwhatsthis.h>
#include <qwidget.h>

Checkbook::Checkbook( QWidget *parent, const QString &n, const QString &fd, char symbol )
	: QDialog( parent, 0, TRUE, WStyle_ContextHelp )
{
	name = n;
	filename = fd;
	filename.append( name );
	filename.append( ".qcb" );
	filedir = fd;
	currencySymbol = symbol;
	currBalance = 0.0;

	if ( name != "" )
	{
		QString tempstr = name;
		tempstr.append( " - " );
		tempstr.append( tr( "Checkbook" ) );
		setCaption( tempstr );
	}
	else
	{
		setCaption( tr( "New checkbook" ) );
	}

	// Setup layout to make everything pretty
	QVBoxLayout *layout = new QVBoxLayout( this );
	layout->setMargin( 2 );
	layout->setSpacing( 4 );

	// Setup tabs for all info
	mainWidget = new OTabWidget( this );
	layout->addWidget( mainWidget );

	mainWidget->addTab( initInfo(), "checkbook/infotab", tr( "Info" ) );
	mainWidget->addTab( initTransactions(), "checkbook/trantab", tr( "Transactions" ) );
	mainWidget->addTab( initCharts(), "checkbook/charttab", tr( "Charts" ) );
	mainWidget->setCurrentTab( tr( "Info" ) );

	// Load checkbook information
	loadCheckbook();
}

Checkbook::~Checkbook()
{
}

const QString &Checkbook::getName()
{
	return( name );
}

QWidget *Checkbook::initInfo()
{
	QWidget *control = new QWidget( mainWidget );

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

	// Account name
	QLabel *label = new QLabel( tr( "Name:" ), container );
	QWhatsThis::add( label, tr( "Enter name of checkbook here." ) );
	layout->addWidget( label, 0, 0 );
	nameEdit = new QLineEdit( container );
	QWhatsThis::add( nameEdit, tr( "Enter name of checkbook here." ) );
	connect( nameEdit, SIGNAL( textChanged( const QString & ) ),
			 this, SLOT( slotNameChanged( const QString & ) ) );
	layout->addWidget( nameEdit, 0, 1 );

	// Type of account
	label = new QLabel( tr( "Type:" ), container );
	QWhatsThis::add( label, tr( "Select type of checkbook here." ) );
	layout->addWidget( label, 1, 0 );
	typeList = new QComboBox( container );
	QWhatsThis::add( typeList, tr( "Select type of checkbook here." ) );
	typeList->insertItem( tr( "Savings" ) );		// 0
	typeList->insertItem( tr( "Checking" ) );		// 1
	typeList->insertItem( tr( "CD" ) );				// 2
	typeList->insertItem( tr( "Money market" ) );	// 3
	typeList->insertItem( tr( "Mutual fund" ) );	// 4
	typeList->insertItem( tr( "Other" ) );			// 5
	layout->addWidget( typeList, 1, 1 );

	// Bank/institution name
	label = new QLabel( tr( "Bank:" ), container );
	QWhatsThis::add( label, tr( "Enter name of the bank for this checkbook here." ) );
	layout->addWidget( label, 2, 0 );
	bankEdit = new QLineEdit( container );
	QWhatsThis::add( bankEdit, tr( "Enter name of the bank for this checkbook here." ) );
	layout->addWidget( bankEdit, 2, 1 );

	// Account number
	label = new QLabel( tr( "Account number:" ), container );
	QWhatsThis::add( label, tr( "Enter account number for this checkbook here." ) );
	layout->addWidget( label, 3, 0 );
	acctNumEdit = new QLineEdit( container );
	QWhatsThis::add( acctNumEdit, tr( "Enter account number for this checkbook here." ) );
	layout->addWidget( acctNumEdit, 3, 1 );

	// PIN number
	label = new QLabel( tr( "PIN number:" ), container );
	QWhatsThis::add( label, tr( "Enter PIN number for this checkbook here." ) );
	layout->addWidget( label, 4, 0 );
	pinNumEdit = new QLineEdit( container );
	QWhatsThis::add( pinNumEdit, tr( "Enter PIN number for this checkbook here." ) );
	layout->addWidget( pinNumEdit, 4, 1 );

	// Starting balance
	label = new QLabel( tr( "Starting balance:" ), container );
	QWhatsThis::add( label, tr( "Enter the initial balance for this checkbook here." ) );
	layout->addWidget( label, 5, 0 );
	balanceEdit = new QLineEdit( container );
	QWhatsThis::add( balanceEdit, tr( "Enter the initial balance for this checkbook here." ) );
	connect( balanceEdit, SIGNAL( textChanged( const QString & ) ),
			 this, SLOT( slotStartingBalanceChanged( const QString & ) ) );
	layout->addWidget( balanceEdit, 5, 1 );

	// Notes
	label = new QLabel( tr( "Notes:" ), container );
	QWhatsThis::add( label, tr( "Enter any additional information for this checkbook here." ) );
	layout->addWidget( label, 6, 0 );
	notesEdit = new QMultiLineEdit( container );
	QWhatsThis::add( notesEdit, tr( "Enter any additional information for this checkbook here." ) );
	notesEdit->setMaximumHeight( 85 );
	layout->addMultiCellWidget( notesEdit, 7, 7, 0, 1 );

	return control;
}

QWidget *Checkbook::initTransactions()
{
	QWidget *control = new QWidget( mainWidget );

	QGridLayout *layout = new QGridLayout( control );
	layout->setSpacing( 2 );
	layout->setMargin( 4 );

	balanceLabel = new QLabel( tr( "Current balance: %10.00" ).arg( currencySymbol ),
								control );
	QWhatsThis::add( balanceLabel, tr( "This area shows the current balance in this checkbook." ) );
	layout->addMultiCellWidget( balanceLabel, 0, 0, 0, 2 );

	tranTable = new QListView( control );
	QWhatsThis::add( tranTable, tr( "This is a listing of all transactions entered for this checkbook.\n\nTo sort entries by a specific field, click on the column name." ) );
	tranTable->addColumn( tr( "ID" ) );
	tranTable->addColumn( tr( "Date" ) );
	tranTable->addColumn( tr( "Description" ) );
	int colnum = tranTable->addColumn( tr( "Amount" ) );
	tranTable->setColumnAlignment( colnum, Qt::AlignRight );
	tranTable->setAllColumnsShowFocus( TRUE );
	layout->addMultiCellWidget( tranTable, 1, 1, 0, 2 );
	QPEApplication::setStylusOperation( tranTable->viewport(), QPEApplication::RightOnHold );
	connect( tranTable, SIGNAL( rightButtonPressed( QListViewItem *, const QPoint &, int ) ),
			 this, SLOT( slotEditTran() ) );

	QPushButton *btn = new QPushButton( Resource::loadPixmap( "new" ), tr( "New" ), control );
	QWhatsThis::add( btn, tr( "Click here to add a new transaction." ) );
	connect( btn, SIGNAL( clicked() ), this, SLOT( slotNewTran() ) );
	layout->addWidget( btn, 2, 0 );

	btn = new QPushButton( Resource::loadPixmap( "edit" ), tr( "Edit" ), control );
	QWhatsThis::add( btn, tr( "Select a transaction and then click here to edit it." ) );
	connect( btn, SIGNAL( clicked() ), this, SLOT( slotEditTran() ) );
	layout->addWidget( btn, 2, 1 );

	btn = new QPushButton( Resource::loadPixmap( "trash" ), tr( "Delete" ), control );
	QWhatsThis::add( btn, tr( "Select a checkbook and then click here to delete it." ) );
	connect( btn, SIGNAL( clicked() ), this, SLOT( slotDeleteTran() ) );
	layout->addWidget( btn, 2, 2 );

	return( control );
}

QWidget *Checkbook::initCharts()
{
	QWidget *control = new QWidget( mainWidget );

	QGridLayout *layout = new QGridLayout( control );
	layout->setSpacing( 2 );
	layout->setMargin( 4 );

/*
	QLabel *label = new QLabel( control );
	label->setText( tr( "Graph type:" ) );
	layout->addWidget( label, 0, 0 );
	graphList = new QComboBox( control );
	graphList->insertItem( tr( "By category" ) );
	graphList->insertItem( tr( "..." ) );
	graphList->insertItem( tr( "..." ) );
	layout->addWidget( graphList, 0, 1 );
*/

	GraphInfo* info = new GraphInfo( GraphInfo::BarChart, 0x0, tr( "Graph Title" ),
									 tr( "X-Axis" ), tr( "Y-Axis" ) );
	graphWidget = new Graph( control, info );
	QWhatsThis::add( graphWidget, tr( "Charting is not implemented yet." ) );
	layout->addMultiCellWidget( graphWidget, 0, 0, 0, 1 );

	QPushButton *btn = new QPushButton( Resource::loadPixmap( "checkbook/drawbtn" ), tr( "Draw" ), control );
	QWhatsThis::add( btn, tr( "Click here to draw the chart." ) );
	connect( btn, SIGNAL( clicked() ), this, SLOT( slotDrawGraph() ) );
	layout->addWidget( btn, 1, 1 );

	return control;
}

void Checkbook::loadCheckbook()
{
	transactions.clear();

	Config config(filename, Config::File);

	// Load info
	config.setGroup( "Account" );
	nameEdit->setText( name );
	QString temptext = config.readEntry( "Type" );
	int i = typeList->count();
	while ( i > 0 )
	{
		i--;
		typeList->setCurrentItem( i );
		if ( typeList->currentText() == temptext )
		{
			break;
		}
	}
	bankEdit->setText( config.readEntry( "Bank", "" ) );
	acctNumEdit->setText( config.readEntryCrypt( "Number", "" ) );
	pinNumEdit->setText( config.readEntryCrypt( "PINNumber", "" ) );
	balanceEdit->setText( config.readEntry( "Balance", "0.0" ) );
	notesEdit->setText( config.readEntry( "Notes", "" ) );

	bool ok;
	currBalance = balanceEdit->text().toFloat( &ok );
	startBalance = currBalance;

	// Load transactions
	TranInfo *tran;
	QString trandesc = "";
	float amount;
	QString stramount;
	for ( int i = 1; trandesc != QString::null; i++ )
	{
		tran = new TranInfo( config, i );
		trandesc = tran->desc();
		if ( trandesc != QString::null )
		{
			currBalance -= tran->fee();
			amount = tran->amount();
			if ( tran->withdrawal() )
			{
				amount *= -1;
			}
			currBalance += amount;
			stramount.sprintf( "%c%.2f", currencySymbol, amount );

			// Add to transaction list
			transactions.append( tran );

			// Add to transaction table
			( void ) new QListViewItem( tranTable, QString::number( i ), tran->datestr(),
										trandesc, stramount );
		}
		else
		{
			delete tran;
		}
	}
	balanceLabel->setText( tr( "Current balance: %1%2" ).arg( currencySymbol ).arg( currBalance, 0, 'f', 2 ) );

	highTranNum = transactions.count();
}

void Checkbook::adjustBalance( float amount )
{
	currBalance += amount;
	balanceLabel->setText( tr( "Current balance: %1%2" ).arg( currencySymbol ).arg( currBalance, 0, 'f', 2 ) );

}

TranInfo *Checkbook::findTranByID( int id )
{
	TranInfo *traninfo = transactions.first();
	while ( traninfo && traninfo->id() != id )
	{
		traninfo = transactions.next();
	}
	return( traninfo );
}

void Checkbook::accept()
{
	QFile f( filename );
	if ( f.exists() )
	{
		f.remove();
	}

	Config *config = new Config(filename, Config::File);

	// Save info
	config->setGroup( "Account" );
	config->writeEntry( "Type", typeList->currentText() );
	config->writeEntry( "Bank", bankEdit->text() );
	config->writeEntryCrypt( "Number", acctNumEdit->text() );
	config->writeEntryCrypt( "PINNumber", pinNumEdit->text() );
	config->writeEntry( "Balance", balanceEdit->text() );
	config->writeEntry( "Notes", notesEdit->text() );

	// Save transactions
	TranInfo *tran = transactions.first();
	int i = 1;
	while ( tran )
	{
		tran->write( config, i );
		tran = transactions.next();
		i++;
	}
	config->write();

	QDialog::accept();
}

void Checkbook::slotNameChanged( const QString &newname )
{
	name = newname;
	filename = filedir;
	filename.append( newname );
	filename.append( ".qcb" );
	QString tempstr = name;
	tempstr.append( " - " );
	tempstr.append( tr( "Checkbook" ) );
	setCaption( tempstr );
}

void Checkbook::slotStartingBalanceChanged( const QString &newbalance )
{
	currBalance -= startBalance;
	bool ok;
	startBalance = newbalance.toFloat( &ok );
	adjustBalance( startBalance );
}

void Checkbook::slotNewTran()
{
	highTranNum++;
	TranInfo *traninfo = new TranInfo( highTranNum );

	Transaction *currtran = new Transaction( this, name,
											 traninfo,
											 currencySymbol );
	currtran->showMaximized();
	if ( currtran->exec() == QDialog::Accepted )
	{
		float amount = traninfo->amount();
		if ( traninfo->withdrawal() )
		{
			amount *= -1;
		}
		QString stramount;
		stramount.sprintf( "%c%.2f", currencySymbol, amount );

		// Add to transaction list
		transactions.append( traninfo );

		// Add to transaction table
		( void ) new QListViewItem( tranTable, QString::number( highTranNum ),
									traninfo->datestr(), traninfo->desc(), stramount );

		adjustBalance( amount );
	}
	else
	{
		highTranNum--;
		delete traninfo;
	}
}

void Checkbook::slotEditTran()
{
	bool ok;
	QListViewItem *curritem = tranTable->currentItem();
	if ( !curritem )
	{
		return;
	}

	int tranid = curritem->text( 0 ).toInt( &ok );
	TranInfo *traninfo = findTranByID( tranid );
	float origamt = traninfo->amount();
	if ( traninfo->withdrawal() )
	{
		origamt *= -1;
	}

	Transaction *currtran = new Transaction( this, name,
											 traninfo,
											 currencySymbol );
	currtran->showMaximized();
	if ( currtran->exec() == QDialog::Accepted )
	{
		curritem->setText( 1, traninfo->datestr() );

		curritem->setText( 2, traninfo->desc() );

		float amount = traninfo->amount();
		if ( traninfo->withdrawal() )
		{
			amount *= -1;
		}
		adjustBalance( origamt * -1 );
		adjustBalance( amount );
		QString stramount;
		stramount.sprintf( "%c%.2f", currencySymbol, amount );
		curritem->setText( 3, stramount );

		balanceLabel->setText( tr( "Current balance: %1%2" ).arg( currencySymbol ).arg( currBalance, 0, 'f', 2 ) );

		delete currtran;
	}
}

void Checkbook::slotDeleteTran()
{
	QListViewItem *curritem = tranTable->currentItem();
	if ( !curritem )
	{
		return;
	}

	bool ok;
	int tranid = curritem->text( 0 ).toInt( &ok );
	TranInfo *traninfo = findTranByID( tranid );

	if ( QPEMessageBox::confirmDelete ( this, tr( "Delete transaction" ), traninfo->desc() ) )
	{
		float amount = traninfo->amount();
		if ( traninfo->withdrawal() )
		{
			amount *= -1;
		}

		transactions.remove( traninfo );
		delete traninfo;
		delete curritem;

		adjustBalance( amount * -1 );
	}
}

void Checkbook::slotDrawGraph()
{
}
