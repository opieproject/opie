/****************************************************************************
** Form implementation generated from reading ui file 'obexftpdialogbase.ui'
**
** Created: Sun Mar 19 16:47:24 2006
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "obexftpdialogbase.h"

#include <qcombobox.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qmultilineedit.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
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
    resize( 221, 396 ); 
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

    Layout9 = new QVBoxLayout; 
    Layout9->setSpacing( 6 );
    Layout9->setMargin( 0 );

    fileList = new QListView( files, "fileList" );
    fileList->addColumn( tr( "Name" ) );
    fileList->addColumn( tr( "Size" ) );
    Layout9->addWidget( fileList );

    fileProgress = new QProgressBar( files, "fileProgress" );
    Layout9->addWidget( fileProgress );

    buttons = new QHBoxLayout; 
    buttons->setSpacing( 6 );
    buttons->setMargin( 0 );

    getButton = new QPushButton( files, "getButton" );
    getButton->setText( tr( "Get file" ) );
    buttons->addWidget( getButton );

    browseOK = new QPushButton( files, "browseOK" );
    browseOK->setText( tr( "Browse" ) );
    buttons->addWidget( browseOK );
    Layout9->addLayout( buttons );

    statusBar = new QLabel( files, "statusBar" );
    statusBar->setText( tr( "" ) );
    Layout9->addWidget( statusBar );
    filesLayout->addLayout( Layout9 );
    obexFtpTab->insertTab( files, tr( "Device" ) );

    localFs = new QWidget( obexFtpTab, "localFs" );
    obexFtpTab->insertTab( localFs, tr( "Local" ) );

    options = new QWidget( obexFtpTab, "options" );

    connRetries = new QLabel( options, "connRetries" );
    connRetries->setGeometry( QRect( 10, 45, 100, 16 ) ); 
    connRetries->setText( tr( "Retry to connect" ) );

    nReries = new QLineEdit( options, "nReries" );
    nReries->setGeometry( QRect( 115, 40, 60, 23 ) ); 

    uuidLabel = new QLabel( options, "uuidLabel" );
    uuidLabel->setGeometry( QRect( 15, 20, 67, 15 ) ); 
    uuidLabel->setText( tr( "uuid type" ) );

    uuidType = new QComboBox( FALSE, options, "uuidType" );
    uuidType->insertItem( tr( "FBS" ) );
    uuidType->insertItem( tr( "S45" ) );
    uuidType->setGeometry( QRect( 85, 10, 100, 30 ) ); 
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
    setTabOrder( fileList, getButton );
    setTabOrder( getButton, browseOK );
    setTabOrder( browseOK, uuidType );
    setTabOrder( uuidType, nReries );
    setTabOrder( nReries, browseLog );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
ObexFtpDialogBase::~ObexFtpDialogBase()
{
    // no need to delete child widgets, Qt does it all for us
}

