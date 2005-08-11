/****************************************************************************
** Form implementation generated from reading ui file 'qsmbbase.ui'
**
** Created: Thu Aug 11 19:30:44 2005
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "qsmbbase.h"

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qtextview.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a FormQPESMBBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
FormQPESMBBase::FormQPESMBBase( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "FormQPESMBBase" );
    resize( 254, 352 ); 
    QFont f( font() );
    setFont( f ); 
    setCaption( tr( "Opie Smb" ) );
    FormQPESMBBaseLayout = new QGridLayout( this ); 
    FormQPESMBBaseLayout->setSpacing( 2 );
    FormQPESMBBaseLayout->setMargin( 0 );

    TabWidget2 = new QTabWidget( this, "TabWidget2" );

    tab = new QWidget( TabWidget2, "tab" );
    tabLayout = new QGridLayout( tab ); 
    tabLayout->setSpacing( 0 );
    tabLayout->setMargin( 2 );

    CBHost = new QComboBox( FALSE, tab, "CBHost" );

    tabLayout->addMultiCellWidget( CBHost, 1, 1, 0, 3 );

    mountpt = new QComboBox( FALSE, tab, "mountpt" );

    tabLayout->addMultiCellWidget( mountpt, 4, 4, 1, 3 );

    LScan = new QLabel( tab, "LScan" );
    LScan->setText( tr( "" ) );

    tabLayout->addWidget( LScan, 0, 3 );

    textLabel1 = new QLabel( tab, "textLabel1" );
    textLabel1->setText( tr( "mount pt:" ) );

    tabLayout->addWidget( textLabel1, 4, 0 );

    ListViewScan = new QListView( tab, "ListViewScan" );
    ListViewScan->addColumn( tr( "Shares" ) );

    tabLayout->addMultiCellWidget( ListViewScan, 2, 2, 0, 3 );

    BtnScan = new QPushButton( tab, "BtnScan" );
    BtnScan->setText( tr( "&Scan" ) );

    tabLayout->addMultiCellWidget( BtnScan, 0, 0, 0, 1 );

    BtnClear = new QPushButton( tab, "BtnClear" );
    BtnClear->setText( tr( "&Clear" ) );

    tabLayout->addWidget( BtnClear, 0, 2 );

    onbootBtn = new QCheckBox( tab, "onbootBtn" );
    onbootBtn->setText( tr( "Reconnect on boot" ) );

    tabLayout->addMultiCellWidget( onbootBtn, 3, 3, 0, 2 );

    DoItBtn = new QPushButton( tab, "DoItBtn" );
    DoItBtn->setText( tr( "&Login" ) );

    tabLayout->addMultiCellWidget( DoItBtn, 5, 5, 0, 1 );

    Layout6 = new QGridLayout; 
    Layout6->setSpacing( 6 );
    Layout6->setMargin( 0 );

    Layout2 = new QHBoxLayout; 
    Layout2->setSpacing( 6 );
    Layout2->setMargin( 0 );

    pwdlabel = new QLabel( tab, "pwdlabel" );
    pwdlabel->setText( tr( "password" ) );
    Layout2->addWidget( pwdlabel );

    password = new QLineEdit( tab, "password" );
    Layout2->addWidget( password );

    Layout6->addLayout( Layout2, 1, 0 );

    Layout5 = new QHBoxLayout; 
    Layout5->setSpacing( 6 );
    Layout5->setMargin( 0 );

    usrlabel = new QLabel( tab, "usrlabel" );
    usrlabel->setText( tr( "username" ) );
    Layout5->addWidget( usrlabel );

    username = new QLineEdit( tab, "username" );
    Layout5->addWidget( username );

    Layout6->addLayout( Layout5, 0, 0 );

    tabLayout->addMultiCellLayout( Layout6, 5, 5, 2, 3 );
    TabWidget2->insertTab( tab, tr( "Scan" ) );

    tab_2 = new QWidget( TabWidget2, "tab_2" );
    tabLayout_2 = new QGridLayout( tab_2 ); 
    tabLayout_2->setSpacing( 6 );
    tabLayout_2->setMargin( 11 );

    TextViewOutput = new QTextView( tab_2, "TextViewOutput" );
    TextViewOutput->setTextFormat( QTextView::PlainText );

    tabLayout_2->addWidget( TextViewOutput, 0, 0 );
    TabWidget2->insertTab( tab_2, tr( "SMB Output" ) );

    FormQPESMBBaseLayout->addWidget( TabWidget2, 0, 0 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
FormQPESMBBase::~FormQPESMBBase()
{
    // no need to delete child widgets, Qt does it all for us
}

