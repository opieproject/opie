/****************************************************************************
** Form implementation generated from reading ui file 'vpnGUI.ui'
**
** Created: Tue Mar 30 02:42:53 2004
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "vpnGUI.h"

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a VPNGUI which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
VPNGUI::VPNGUI( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "VPN_FRM" );
    resize( 283, 199 ); 
    setCaption( tr( "VPN" ) );
    VPN_FRMLayout = new QVBoxLayout( this ); 
    VPN_FRMLayout->setSpacing( 2 );
    VPN_FRMLayout->setMargin( 2 );

    Layout4 = new QHBoxLayout; 
    Layout4->setSpacing( 6 );
    Layout4->setMargin( 0 );

    TextLabel4 = new QLabel( this, "TextLabel4" );
    TextLabel4->setText( tr( "Name" ) );
    Layout4->addWidget( TextLabel4 );

    Name_LE = new QLineEdit( this, "Name_LE" );
    Layout4->addWidget( Name_LE );
    VPN_FRMLayout->addLayout( Layout4 );

    TextLabel3 = new QLabel( this, "TextLabel3" );
    TextLabel3->setText( tr( "Description" ) );
    VPN_FRMLayout->addWidget( TextLabel3 );

    Description_LE = new QMultiLineEdit( this, "Description_LE" );
    VPN_FRMLayout->addWidget( Description_LE );

    Layout5 = new QHBoxLayout; 
    Layout5->setSpacing( 6 );
    Layout5->setMargin( 0 );

    Automatic_CB = new QCheckBox( this, "Automatic_CB" );
    Automatic_CB->setText( tr( "Start automatically" ) );
    Layout5->addWidget( Automatic_CB );

    Confirm_CB = new QCheckBox( this, "Confirm_CB" );
    Confirm_CB->setText( tr( "Confirm before start" ) );
    Layout5->addWidget( Confirm_CB );
    VPN_FRMLayout->addLayout( Layout5 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
VPNGUI::~VPNGUI()
{
    // no need to delete child widgets, Qt does it all for us
}

