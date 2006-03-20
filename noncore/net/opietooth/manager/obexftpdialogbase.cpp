/****************************************************************************
** Form implementation generated from reading ui file 'obexftpdialogbase.ui'
**
** Created: Tue Mar 21 00:29:33 2006
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "obexftpdialogbase.h"

#include <qcombobox.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qmultilineedit.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a ObexFtpDialogBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ObexFtpDialogBase::ObexFtpDialogBase( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "ObexFtpDialogBase" );
    resize( 267, 312 ); 
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, sizePolicy().hasHeightForWidth() ) );
    setCaption( tr( "Browse device" ) );
    ObexFtpDialogBaseLayout = new QVBoxLayout( this ); 
    ObexFtpDialogBaseLayout->setSpacing( 0 );
    ObexFtpDialogBaseLayout->setMargin( 0 );

    obexFtpTab = new QTabWidget( this, "obexFtpTab" );

    files = new QWidget( obexFtpTab, "files" );
    filesLayout = new QVBoxLayout( files ); 
    filesLayout->setSpacing( 0 );
    filesLayout->setMargin( 0 );

    Layout13 = new QGridLayout; 
    Layout13->setSpacing( 6 );
    Layout13->setMargin( 0 );

    fileList = new QListView( files, "fileList" );
    fileList->addColumn( tr( "Name" ) );
    fileList->addColumn( tr( "Size" ) );

    Layout13->addWidget( fileList, 0, 0 );

    fileProgress = new QProgressBar( files, "fileProgress" );

    Layout13->addWidget( fileProgress, 1, 0 );

    Layout11 = new QHBoxLayout; 
    Layout11->setSpacing( 6 );
    Layout11->setMargin( 0 );

    browseOK = new QPushButton( files, "browseOK" );
    browseOK->setText( tr( "Browse" ) );
    Layout11->addWidget( browseOK );

    getButton = new QPushButton( files, "getButton" );
    getButton->setText( tr( "Get file" ) );
    Layout11->addWidget( getButton );

    putButton = new QPushButton( files, "putButton" );
    putButton->setText( tr( "Put file" ) );
    Layout11->addWidget( putButton );

    Layout13->addLayout( Layout11, 2, 0 );

    statusBar = new QLabel( files, "statusBar" );
    statusBar->setText( tr( "" ) );

    Layout13->addWidget( statusBar, 3, 0 );
    filesLayout->addLayout( Layout13 );
    obexFtpTab->insertTab( files, tr( "Device" ) );

    localFs = new QWidget( obexFtpTab, "localFs" );
    obexFtpTab->insertTab( localFs, tr( "Local" ) );

    options = new QWidget( obexFtpTab, "options" );

    QWidget* privateLayoutWidget = new QWidget( options, "Layout5" );
    privateLayoutWidget->setGeometry( QRect( 45, 5, 162, 63 ) ); 
    Layout5 = new QVBoxLayout( privateLayoutWidget ); 
    Layout5->setSpacing( 6 );
    Layout5->setMargin( 0 );

    Layout3 = new QHBoxLayout; 
    Layout3->setSpacing( 6 );
    Layout3->setMargin( 0 );

    uuidLabel = new QLabel( privateLayoutWidget, "uuidLabel" );
    uuidLabel->setText( tr( "uuid type" ) );
    Layout3->addWidget( uuidLabel );

    uuidType = new QComboBox( FALSE, privateLayoutWidget, "uuidType" );
    uuidType->insertItem( tr( "FBS" ) );
    uuidType->insertItem( tr( "S45" ) );
    Layout3->addWidget( uuidType );
    Layout5->addLayout( Layout3 );

    Layout4 = new QHBoxLayout; 
    Layout4->setSpacing( 6 );
    Layout4->setMargin( 0 );

    connRetries = new QLabel( privateLayoutWidget, "connRetries" );
    connRetries->setText( tr( "Retry to connect" ) );
    Layout4->addWidget( connRetries );

    nReries = new QSpinBox( privateLayoutWidget, "nReries" );
    nReries->setButtonSymbols( QSpinBox::PlusMinus );
    Layout4->addWidget( nReries );
    Layout5->addLayout( Layout4 );
    obexFtpTab->insertTab( options, tr( "Options" ) );

    browse = new QWidget( obexFtpTab, "browse" );
    browseLayout = new QHBoxLayout( browse ); 
    browseLayout->setSpacing( 0 );
    browseLayout->setMargin( 0 );

    browseLog = new QMultiLineEdit( browse, "browseLog" );
    browseLayout->addWidget( browseLog );
    obexFtpTab->insertTab( browse, tr( "Log" ) );
    ObexFtpDialogBaseLayout->addWidget( obexFtpTab );

    // tab order
    setTabOrder( obexFtpTab, fileList );
    setTabOrder( fileList, browseOK );
    setTabOrder( browseOK, getButton );
    setTabOrder( getButton, putButton );
    setTabOrder( putButton, uuidType );
    setTabOrder( uuidType, browseLog );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
ObexFtpDialogBase::~ObexFtpDialogBase()
{
    // no need to delete child widgets, Qt does it all for us
}

