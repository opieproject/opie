/****************************************************************************
** Form implementation generated from reading ui file 'interfaceedit.ui'
**
** Created: Mon Sep 23 12:18:55 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "interfaceedit.h"

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include "qwidget.h"
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a InterfaceConfiguration which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
InterfaceConfiguration::InterfaceConfiguration( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "InterfaceConfiguration" );
    resize( 177, 306 ); 
    setCaption( tr( "Interface Configuration" ) );
    InterfaceConfigurationLayout = new QGridLayout( this ); 
    InterfaceConfigurationLayout->setSpacing( 6 );
    InterfaceConfigurationLayout->setMargin( 11 );

    profile = new QComboBox( FALSE, this, "profile" );
    profile->insertItem( tr( "All" ) );

    InterfaceConfigurationLayout->addWidget( profile, 2, 1 );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setText( tr( "Profile:" ) );

    InterfaceConfigurationLayout->addWidget( TextLabel1, 2, 0 );

    Line1 = new QFrame( this, "Line1" );
    Line1->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    InterfaceConfigurationLayout->addMultiCellWidget( Line1, 1, 1, 0, 1 );

    CheckBox3 = new QCheckBox( this, "CheckBox3" );
    CheckBox3->setText( tr( "Automaticly bring up" ) );

    InterfaceConfigurationLayout->addMultiCellWidget( CheckBox3, 0, 0, 0, 1 );

    dhcpCheckBox = new QCheckBox( this, "dhcpCheckBox" );
    dhcpCheckBox->setText( tr( "DHCP" ) );

    InterfaceConfigurationLayout->addMultiCellWidget( dhcpCheckBox, 3, 3, 0, 1 );

    TextLabel3_3_2 = new QLabel( this, "TextLabel3_3_2" );
    TextLabel3_3_2->setText( tr( "Lease Hours" ) );

    InterfaceConfigurationLayout->addWidget( TextLabel3_3_2, 4, 0 );

    SpinBox1_2 = new QSpinBox( this, "SpinBox1_2" );
    SpinBox1_2->setMaxValue( 336 );
    SpinBox1_2->setMinValue( 1 );
    SpinBox1_2->setValue( 24 );

    InterfaceConfigurationLayout->addWidget( SpinBox1_2, 4, 1 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    InterfaceConfigurationLayout->addItem( spacer, 11, 1 );

    TextLabel4 = new QLabel( this, "TextLabel4" );
    TextLabel4->setText( tr( "IP Address" ) );

    InterfaceConfigurationLayout->addWidget( TextLabel4, 6, 0 );

    ipAddressEdit = new QLineEdit( this, "ipAddressEdit" );

    InterfaceConfigurationLayout->addWidget( ipAddressEdit, 6, 1 );

    TextLabel5 = new QLabel( this, "TextLabel5" );
    TextLabel5->setText( tr( "Subnet Mask" ) );

    InterfaceConfigurationLayout->addWidget( TextLabel5, 7, 0 );

    firstDNSLineEdit = new QLineEdit( this, "firstDNSLineEdit" );

    InterfaceConfigurationLayout->addWidget( firstDNSLineEdit, 9, 1 );

    TextLabel3 = new QLabel( this, "TextLabel3" );
    TextLabel3->setText( tr( "Second DNS" ) );

    InterfaceConfigurationLayout->addWidget( TextLabel3, 10, 0 );

    subnetMaskEdit = new QLineEdit( this, "subnetMaskEdit" );

    InterfaceConfigurationLayout->addWidget( subnetMaskEdit, 7, 1 );

    gatewayEdit = new QLineEdit( this, "gatewayEdit" );

    InterfaceConfigurationLayout->addWidget( gatewayEdit, 8, 1 );

    TextLabel7 = new QLabel( this, "TextLabel7" );
    TextLabel7->setText( tr( "Gateway" ) );

    InterfaceConfigurationLayout->addWidget( TextLabel7, 8, 0 );

    TextLabel2 = new QLabel( this, "TextLabel2" );
    TextLabel2->setText( tr( "First DNS" ) );

    InterfaceConfigurationLayout->addWidget( TextLabel2, 9, 0 );

    secondDNSLineEdit = new QLineEdit( this, "secondDNSLineEdit" );

    InterfaceConfigurationLayout->addWidget( secondDNSLineEdit, 10, 1 );

    GroupBox2 = new QGroupBox( this, "GroupBox2" );
    GroupBox2->setTitle( tr( "Static Ip Configuration" ) );

    InterfaceConfigurationLayout->addMultiCellWidget( GroupBox2, 5, 5, 0, 1 );

    // signals and slots connections
    connect( dhcpCheckBox, SIGNAL( toggled(bool) ), SpinBox1_2, SLOT( setEnabled(bool) ) );
    connect( dhcpCheckBox, SIGNAL( toggled(bool) ), GroupBox2, SLOT( setDisabled(bool) ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
InterfaceConfiguration::~InterfaceConfiguration()
{
    // no need to delete child widgets, Qt does it all for us
}

