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

#include "transaction.h"
#include "traninfo.h"
#include "cfg.h"
#include "checkbook.h"

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/datebookmonth.h>
using namespace Opie::Core;

/* QT */
#include <qbuttongroup.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qradiobutton.h>
#include <qwhatsthis.h>

Transaction::Transaction( QWidget *parent, bool bNew, const QString &acctname,
							 TranInfo *info, Cfg *pCfg )
	: QDialog( parent, 0, TRUE, WStyle_ContextHelp )
{
	QString tempstr = tr( "Transaction for " );
	tempstr.append( acctname );
	setCaption( tempstr );

    _bNew=bNew;
	tran = info;
	_pCfg=pCfg;

	QVBoxLayout *vb = new QVBoxLayout( this );

	QScrollView *sv = new QScrollView( this );
	vb->addWidget( sv, 0, 0 );
	sv->setResizePolicy( QScrollView::AutoOneFit );
	sv->setFrameStyle( QFrame::NoFrame );

	QWidget *container = new QWidget( sv->viewport() );
	sv->addChild( container );

	QGridLayout *layout = new QGridLayout( container );
	layout->setSpacing( 2 );
	layout->setMargin( 4 );

	// Withdrawal/Deposit
	QButtonGroup *btngrp = new QButtonGroup( container );
	btngrp->setColumnLayout(0, Qt::Vertical );
	btngrp->layout()->setSpacing( 0 );
	btngrp->layout()->setMargin( 0 );
	btngrp->setMaximumWidth( 220 );
	QGridLayout *layout2 = new QGridLayout( btngrp->layout() );
	layout2->setSpacing( 2 );
	layout2->setMargin( 2 );
	withBtn = new QRadioButton( tr( "Withdrawal" ), btngrp );
	QWhatsThis::add( withBtn, tr( "Select whether the transaction is a withdrawal or deposit here." ) );
	layout2->addWidget( withBtn, 0, 0 );
	connect( withBtn, SIGNAL( clicked() ), this, SLOT( slotWithdrawalClicked() ) );
	depBtn = new QRadioButton( tr( "Deposit" ), btngrp );
	QWhatsThis::add( depBtn, tr( "Select whether the transaction is a withdrawal or deposit here." ) );
	layout2->addWidget( depBtn, 0, 1 );
	btngrp->setMaximumSize( 320, withBtn->height() );
	connect( depBtn, SIGNAL( clicked() ), this, SLOT( slotDepositClicked() ) );
	layout->addMultiCellWidget( btngrp, 0, 0, 0, 3 );

	// Date
	QLabel *label = new QLabel( tr( "Date:" ), container );
	QWhatsThis::add( label, tr( "Select date of transaction here." ) );
	layout->addWidget( label, 1, 0 );
	dateBtn = new QPushButton( TimeString::shortDate( QDate::currentDate() ),
								container );
	QWhatsThis::add( dateBtn, tr( "Select date of transaction here." ) );
	QPopupMenu *m1 = new QPopupMenu( container );
	datePicker = new DateBookMonth( m1, 0, TRUE );
	m1->insertItem( datePicker );
	dateBtn->setPopup( m1 );
	connect( datePicker, SIGNAL( dateClicked(int,int,int) ),
			 this, SLOT( slotDateChanged(int,int,int) ) );
	layout->addWidget( dateBtn, 1, 1 );

	// Check number
	label = new QLabel( tr( "Number:" ), container );
	QWhatsThis::add( label, tr( "Enter check number here." ) );
	layout->addWidget( label, 1, 2 );
	numEdit = new QLineEdit( container );
	QWhatsThis::add( numEdit, tr( "Enter check number here." ) );
	numEdit->setMaximumWidth( 40 );
	layout->addWidget( numEdit, 1, 3 );

	// Description
	label = new QLabel( tr( "Description:" ), container );
	QWhatsThis::add( label, tr( "Enter description of transaction here." ) );
	layout->addWidget( label, 2, 0 );
    _cbDesc=new QComboBox( true, container );
    _cbDesc->insertStringList( _pCfg->getPayees() );
	QWhatsThis::add( _cbDesc, tr( "Enter description of transaction here." ) );
	layout->addMultiCellWidget( _cbDesc, 2, 2, 1, 3 );
    connect( _cbDesc, SIGNAL( activated(const QString&) ), this, SLOT( slotActivated(const QString&) ) );


	// Category
	label = new QLabel( tr( "Category:" ), container );
	QWhatsThis::add( label, tr( "Select transaction category here." ) );
	layout->addWidget( label, 3, 0 );
	catList = new QComboBox( container );
	QWhatsThis::add( catList, tr( "Select transaction category here." ) );
	layout->addMultiCellWidget( catList, 3, 3, 1, 3 );

	// Type
	label = new QLabel( tr( "Type:" ), container );
	QWhatsThis::add( label, tr( "Select transaction type here.\n\nThe options available vary based on whether the transaction is a deposit or withdrawal." ) );
	layout->addWidget( label, 4, 0 );
	typeList = new QComboBox( container );
	QWhatsThis::add( typeList, tr( "Select transaction type here.\n\nThe options available vary based on whether the transaction is a deposit or withdrawal." ) );
	layout->addMultiCellWidget( typeList, 4, 4, 1, 3 );


	// Amount
	label = new QLabel( tr( "Amount:" ), container );
	QWhatsThis::add( label, tr( "Enter the amount of transaction here.\n\nThe value entered should always be positive." ) );
	layout->addWidget( label, 5, 0 );
	amtEdit = new QLineEdit( container );
	QWhatsThis::add( amtEdit, tr( "Enter the amount of transaction here.\n\nThe value entered should always be positive." ) );
	layout->addMultiCellWidget( amtEdit, 5, 5, 1, 3 );

	// Fee
	label = new QLabel( tr( "Fee:" ), container );
	QWhatsThis::add( label, tr( "Enter any fee amount assoiciated with this transaction.\n\nThe value entered should always be positive." ) );
	layout->addWidget( label, 6, 0 );
	feeEdit = new QLineEdit( container );
	QWhatsThis::add( feeEdit, tr( "Enter any fee amount assoiciated with this transaction.\n\nThe value entered should always be positive." ) );
	layout->addMultiCellWidget( feeEdit, 6, 6, 1, 3 );

	// Notes
	label = new QLabel( tr( "Notes:" ), container );
	QWhatsThis::add( label, tr( "Enter any additional information for this transaction here." ) );
	layout->addWidget( label, 7, 0 );
	noteEdit = new QMultiLineEdit( container );
	QWhatsThis::add( noteEdit, tr( "Enter any additional information for this transaction here." ) );
	layout->addMultiCellWidget( noteEdit, 8, 8, 0, 3 );

    // init date
    initFromInfo( info );

    // not new handlers
    connect( withBtn, SIGNAL( toggled(bool) ), this, SLOT( slotNotNew() ) );
    connect( depBtn, SIGNAL( toggled(bool) ), this, SLOT( slotNotNew() ) );
    connect( catList, SIGNAL(activated(const QString&)), this, SLOT( slotNotNew() ) );
    connect( typeList, SIGNAL(activated(const QString&)), this, SLOT( slotNotNew() ) );
    connect( amtEdit, SIGNAL(textChanged(const QString&)), this, SLOT( slotNotNew() ) );
    connect( feeEdit, SIGNAL(textChanged(const QString&)), this, SLOT( slotNotNew() ) );
    connect( noteEdit, SIGNAL(textChanged()), this, SLOT( slotNotNew() ) );
}

// --- initFromInfo -----------------------------------------------------------
void Transaction::initFromInfo(TranInfo *info, bool bPopulateOld)
{
	// Populate current values if provided
	if ( info )
	{
		if ( info->withdrawal() )
		{
			withBtn->setChecked( TRUE );
			slotWithdrawalClicked();
		}
		else
		{
			depBtn->setChecked( TRUE );
			slotDepositClicked();
		}

        if( !bPopulateOld ) {
		    QDate dt = info->date();
		    slotDateChanged( dt.year(), dt.month(), dt.day() );
		    datePicker->setDate( dt );
		    numEdit->setText( info->number() );
        }
		QString temptext = info->category();

        // set description field
        int i;
        for(i=_cbDesc->count()-1; i>=0; i--) {
            if( _cbDesc->text(i)==info->desc() ) {
                _cbDesc->setCurrentItem(i);
                break;
            }
        }
        if( i<=0 )
            _cbDesc->setEditText( info->desc() );

		i = catList->count();
		while ( i > 0 )
		{
			i--;
			catList->setCurrentItem( i );
			if ( catList->currentText() == temptext )
			{
				break;
			}
		}
		temptext = info->type();
		i = typeList->count();
		while ( i > 0  )
		{
			i--;
			typeList->setCurrentItem( i );
			if ( typeList->currentText() == temptext )
			{
				break;
			}
		}
		amtEdit->setText( QString( "%1" ).arg( info->amount(), 0, 'f', 2 ) );
		feeEdit->setText( QString( "%1" ).arg( info->fee(), 0, 'f', 2 ) );
		noteEdit->setText( info->notes() );
	}
	else
	{
		withBtn->setChecked( TRUE );
	}
}


// --- ~Transaction -----------------------------------------------------------
Transaction::~Transaction()
{
}

// --- accept -----------------------------------------------------------------
void Transaction::accept()
{
	tran->setDesc( _cbDesc->currentText() );
	tran->setDate( datePicker->selectedDate() );
	tran->setWithdrawal( withBtn->isChecked() );
	tran->setType( typeList->currentText() );
	tran->setCategory( catList->currentText() );
	bool ok;
	tran->setAmount( amtEdit->text().toFloat( &ok ) );
	tran->setFee( feeEdit->text().toFloat( &ok ) );
	tran->setNumber( numEdit->text() );
	tran->setNotes( noteEdit->text() );

	QDialog::accept();
}

void Transaction::slotWithdrawalClicked()
{
	catList->clear();
    CategoryList *pCatList=_pCfg->getCategoryList();
    for(Category *pCat=pCatList->first(); pCat; pCat=pCatList->next()) {
        if( !pCat->isIncome() )
            catList->insertItem( pCat->getName() );
    }
    catList->setCurrentItem(0);

	typeList->clear();
	typeList->insertItem( tr( "Debit Charge" ) );
	typeList->insertItem( tr( "Written Check" ) );
	typeList->insertItem( tr( "Transfer" ) );
	typeList->insertItem( tr( "Credit Card" ) );
}

void Transaction::slotDepositClicked()
{
	catList->clear();
	CategoryList *pCatList=_pCfg->getCategoryList();
    for(Category *pCat=pCatList->first(); pCat; pCat=pCatList->next()) {
        if( pCat->isIncome() )
            catList->insertItem( pCat->getName() );
    }
    catList->setCurrentItem( 0 );

	typeList->clear();
	typeList->insertItem( tr( "Written Check" ) );
	typeList->insertItem( tr( "Automatic Payment" ) );
	typeList->insertItem( tr( "Transfer" ) );
	typeList->insertItem( tr( "Cash" ) );
}

// --- slotDateChanged --------------------------------------------------------
void Transaction::slotDateChanged( int y, int m, int d )
{
	QDate date;
	date.setYMD( y, m, d );
	dateBtn->setText( TimeString::shortDate( date ) );
}



// --- slotActivated ----------------------------------------------------------
// Search for the most recent transaction with this description/payee and
// fill amount etc here, as long the new flag is set
void Transaction::slotActivated(const QString &arg )
{
    if( !_bNew ) return;
    TranInfoList *pTl=((Checkbook *)parentWidget())->getTranList();
    if( pTl ) {
        TranInfo *pTi=pTl->findMostRecentByDesc( arg );
        if( pTi ) {
            initFromInfo( pTi, true );
            amtEdit->setFocus();
            amtEdit->setSelection(0, amtEdit->text().length() );
            amtEdit->setCursorPosition(0);
        }
    }
}

// slotNotNew -----------------------------------------------------------------
void Transaction::slotNotNew()
{
    odebug << "Not new" << oendl; 
    _bNew=false;
}
