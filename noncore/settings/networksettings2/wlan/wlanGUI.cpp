/****************************************************************************
** Form implementation generated from reading ui file 'wlanGUI.ui'
**
** Created: Tue Mar 30 02:42:58 2004
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "wlanGUI.h"

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a WLanGUI which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
WLanGUI::WLanGUI( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "WLanGUI" );
    resize( 196, 329 ); 
    setCaption( tr( "Form1" ) );
    WLanGUILayout = new QVBoxLayout( this ); 
    WLanGUILayout->setSpacing( 0 );
    WLanGUILayout->setMargin( 0 );

    tabWidget = new QTabWidget( this, "tabWidget" );
    tabWidget->setEnabled( TRUE );

    ConfigPage = new QWidget( tabWidget, "ConfigPage" );
    ConfigPageLayout = new QVBoxLayout( ConfigPage ); 
    ConfigPageLayout->setSpacing( 2 );
    ConfigPageLayout->setMargin( 0 );

    Layout6 = new QGridLayout; 
    Layout6->setSpacing( 6 );
    Layout6->setMargin( 1 );

    essidLabel = new QLabel( ConfigPage, "essidLabel" );
    essidLabel->setEnabled( TRUE );
    essidLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, essidLabel->sizePolicy().hasHeightForWidth() ) );
    essidLabel->setText( tr( "ESS-ID" ) );

    Layout6->addWidget( essidLabel, 1, 0 );

    essid = new QComboBox( FALSE, ConfigPage, "essid" );
    essid->insertItem( tr( "any" ) );
    essid->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, essid->sizePolicy().hasHeightForWidth() ) );
    essid->setEditable( TRUE );
    essid->setCurrentItem( 0 );
    essid->setSizeLimit( 5 );
    essid->setAutoCompletion( TRUE );
    essid->setDuplicatesEnabled( FALSE );

    Layout6->addWidget( essid, 1, 1 );

    modeLabel = new QLabel( ConfigPage, "modeLabel" );
    modeLabel->setEnabled( TRUE );
    modeLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, modeLabel->sizePolicy().hasHeightForWidth() ) );
    modeLabel->setText( tr( "Mode" ) );

    Layout6->addWidget( modeLabel, 0, 0 );

    mode = new QComboBox( FALSE, ConfigPage, "mode" );
    mode->insertItem( tr( "Infrastructure" ) );
    mode->insertItem( tr( "Auto" ) );
    mode->insertItem( tr( "Managed" ) );
    mode->insertItem( tr( "Ad-Hoc" ) );
    mode->setEnabled( TRUE );
    mode->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, mode->sizePolicy().hasHeightForWidth() ) );

    Layout6->addWidget( mode, 0, 1 );
    ConfigPageLayout->addLayout( Layout6 );

    Layout8 = new QGridLayout; 
    Layout8->setSpacing( 2 );
    Layout8->setMargin( 1 );

    specifyAp = new QCheckBox( ConfigPage, "specifyAp" );
    specifyAp->setText( tr( "Specify &Access Point" ) );

    Layout8->addMultiCellWidget( specifyAp, 0, 0, 0, 2 );

    macLabel = new QLabel( ConfigPage, "macLabel" );
    macLabel->setEnabled( FALSE );
    macLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)1, macLabel->sizePolicy().hasHeightForWidth() ) );
    macLabel->setText( tr( "MAC" ) );

    Layout8->addWidget( macLabel, 1, 1 );

    macEdit = new QLineEdit( ConfigPage, "macEdit" );
    macEdit->setEnabled( FALSE );
    macEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)1, macEdit->sizePolicy().hasHeightForWidth() ) );

    Layout8->addWidget( macEdit, 1, 2 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    Layout8->addItem( spacer, 1, 0 );
    ConfigPageLayout->addLayout( Layout8 );

    Layout7 = new QHBoxLayout; 
    Layout7->setSpacing( 6 );
    Layout7->setMargin( 1 );

    specifyChan = new QCheckBox( ConfigPage, "specifyChan" );
    specifyChan->setText( tr( "Specific &Channel" ) );
    Layout7->addWidget( specifyChan );

    networkChannel = new QSpinBox( ConfigPage, "networkChannel" );
    networkChannel->setEnabled( FALSE );
    networkChannel->setMaxValue( 15 );
    networkChannel->setMinValue( 1 );
    networkChannel->setValue( 1 );
    Layout7->addWidget( networkChannel );
    ConfigPageLayout->addLayout( Layout7 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    ConfigPageLayout->addItem( spacer_2 );
    tabWidget->insertTab( ConfigPage, tr( "General" ) );

    WepPage = new QWidget( tabWidget, "WepPage" );
    WepPageLayout = new QVBoxLayout( WepPage ); 
    WepPageLayout->setSpacing( 2 );
    WepPageLayout->setMargin( 0 );

    wepEnabled = new QCheckBox( WepPage, "wepEnabled" );
    wepEnabled->setText( tr( "&Enable Encryption" ) );
    WepPageLayout->addWidget( wepEnabled );

    KeyButtonGroup = new QButtonGroup( WepPage, "KeyButtonGroup" );
    KeyButtonGroup->setEnabled( FALSE );
    KeyButtonGroup->setTitle( tr( "&Key Setting" ) );
    KeyButtonGroup->setColumnLayout(0, Qt::Vertical );
    KeyButtonGroup->layout()->setSpacing( 0 );
    KeyButtonGroup->layout()->setMargin( 0 );
    KeyButtonGroupLayout = new QGridLayout( KeyButtonGroup->layout() );
    KeyButtonGroupLayout->setAlignment( Qt::AlignTop );
    KeyButtonGroupLayout->setSpacing( 2 );
    KeyButtonGroupLayout->setMargin( 2 );

    keyRadio0 = new QRadioButton( KeyButtonGroup, "keyRadio0" );
    keyRadio0->setText( tr( "Key &1" ) );
    keyRadio0->setChecked( TRUE );

    KeyButtonGroupLayout->addWidget( keyRadio0, 0, 0 );

    keyRadio3 = new QRadioButton( KeyButtonGroup, "keyRadio3" );
    keyRadio3->setText( tr( "Key &4" ) );

    KeyButtonGroupLayout->addWidget( keyRadio3, 3, 0 );

    LineEdit6 = new QLineEdit( KeyButtonGroup, "LineEdit6" );

    KeyButtonGroupLayout->addWidget( LineEdit6, 0, 1 );

    keyRadio1 = new QRadioButton( KeyButtonGroup, "keyRadio1" );
    keyRadio1->setText( tr( "Key &2" ) );

    KeyButtonGroupLayout->addWidget( keyRadio1, 1, 0 );

    keyRadio2 = new QRadioButton( KeyButtonGroup, "keyRadio2" );
    keyRadio2->setText( tr( "Key &3" ) );

    KeyButtonGroupLayout->addWidget( keyRadio2, 2, 0 );

    LineEdit6_2 = new QLineEdit( KeyButtonGroup, "LineEdit6_2" );

    KeyButtonGroupLayout->addWidget( LineEdit6_2, 1, 1 );

    LineEdit6_4 = new QLineEdit( KeyButtonGroup, "LineEdit6_4" );

    KeyButtonGroupLayout->addWidget( LineEdit6_4, 3, 1 );

    LineEdit6_3 = new QLineEdit( KeyButtonGroup, "LineEdit6_3" );

    KeyButtonGroupLayout->addWidget( LineEdit6_3, 2, 1 );
    WepPageLayout->addWidget( KeyButtonGroup );

    NonEncButtonGroup = new QButtonGroup( WepPage, "NonEncButtonGroup" );
    NonEncButtonGroup->setEnabled( FALSE );
    NonEncButtonGroup->setTitle( tr( "Non-encrypted Packets" ) );
    NonEncButtonGroup->setRadioButtonExclusive( TRUE );
    NonEncButtonGroup->setColumnLayout(0, Qt::Vertical );
    NonEncButtonGroup->layout()->setSpacing( 0 );
    NonEncButtonGroup->layout()->setMargin( 0 );
    NonEncButtonGroupLayout = new QGridLayout( NonEncButtonGroup->layout() );
    NonEncButtonGroupLayout->setAlignment( Qt::AlignTop );
    NonEncButtonGroupLayout->setSpacing( 6 );
    NonEncButtonGroupLayout->setMargin( 2 );

    acceptNonEnc = new QRadioButton( NonEncButtonGroup, "acceptNonEnc" );
    acceptNonEnc->setText( tr( "&Accept" ) );
    acceptNonEnc->setChecked( TRUE );
    NonEncButtonGroup->insert( acceptNonEnc, 0 );

    NonEncButtonGroupLayout->addWidget( acceptNonEnc, 0, 0 );

    rejectNonEnc = new QRadioButton( NonEncButtonGroup, "rejectNonEnc" );
    rejectNonEnc->setText( tr( "&Reject" ) );
    NonEncButtonGroup->insert( rejectNonEnc, 1 );

    NonEncButtonGroupLayout->addWidget( rejectNonEnc, 0, 1 );
    WepPageLayout->addWidget( NonEncButtonGroup );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    WepPageLayout->addItem( spacer_3 );
    tabWidget->insertTab( WepPage, tr( "Encryption" ) );

    tab = new QWidget( tabWidget, "tab" );
    tabLayout = new QVBoxLayout( tab ); 
    tabLayout->setSpacing( 2 );
    tabLayout->setMargin( 0 );

    Layout12 = new QGridLayout; 
    Layout12->setSpacing( 2 );
    Layout12->setMargin( 0 );

    TextLabel1_3 = new QLabel( tab, "TextLabel1_3" );
    TextLabel1_3->setText( tr( "Station" ) );

    Layout12->addWidget( TextLabel1_3, 0, 0 );

    Channel_LBL = new QLabel( tab, "Channel_LBL" );
    Channel_LBL->setFrameShape( QLabel::Panel );
    Channel_LBL->setFrameShadow( QLabel::Sunken );

    Layout12->addWidget( Channel_LBL, 3, 1 );

    TextLabel4_3 = new QLabel( tab, "TextLabel4_3" );
    TextLabel4_3->setText( tr( "Channel" ) );

    Layout12->addWidget( TextLabel4_3, 3, 0 );

    TextLabel3_3 = new QLabel( tab, "TextLabel3_3" );
    TextLabel3_3->setText( tr( "Mode" ) );

    Layout12->addWidget( TextLabel3_3, 2, 0 );

    TextLabel2_3 = new QLabel( tab, "TextLabel2_3" );
    TextLabel2_3->setText( tr( "ESSID" ) );

    Layout12->addWidget( TextLabel2_3, 1, 0 );

    TextLabel9_2 = new QLabel( tab, "TextLabel9_2" );
    TextLabel9_2->setText( tr( "AP" ) );

    Layout12->addWidget( TextLabel9_2, 4, 0 );

    Station_LBL = new QLabel( tab, "Station_LBL" );
    Station_LBL->setFrameShape( QLabel::Panel );
    Station_LBL->setFrameShadow( QLabel::Sunken );

    Layout12->addWidget( Station_LBL, 0, 1 );

    Rate_LBL = new QLabel( tab, "Rate_LBL" );
    Rate_LBL->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)1, Rate_LBL->sizePolicy().hasHeightForWidth() ) );
    Rate_LBL->setFrameShape( QLabel::Panel );
    Rate_LBL->setFrameShadow( QLabel::Sunken );

    Layout12->addWidget( Rate_LBL, 5, 1 );

    TextLabel4_2_2 = new QLabel( tab, "TextLabel4_2_2" );
    TextLabel4_2_2->setText( tr( "Rate" ) );

    Layout12->addWidget( TextLabel4_2_2, 5, 0 );

    AP_LBL = new QLabel( tab, "AP_LBL" );
    AP_LBL->setFrameShape( QLabel::Panel );
    AP_LBL->setFrameShadow( QLabel::Sunken );

    Layout12->addWidget( AP_LBL, 4, 1 );

    essidLabel_3 = new QLabel( tab, "essidLabel_3" );
    essidLabel_3->setFrameShape( QLabel::Panel );
    essidLabel_3->setFrameShadow( QLabel::Sunken );

    Layout12->addWidget( essidLabel_3, 1, 1 );

    modeLabel_3 = new QLabel( tab, "modeLabel_3" );
    modeLabel_3->setFrameShape( QLabel::Panel );
    modeLabel_3->setFrameShadow( QLabel::Sunken );

    Layout12->addWidget( modeLabel_3, 2, 1 );
    tabLayout->addLayout( Layout12 );

    GroupBox1 = new QGroupBox( tab, "GroupBox1" );
    GroupBox1->setTitle( tr( "Link Quality" ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 0 );
    GroupBox1->layout()->setMargin( 0 );
    GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );
    GroupBox1Layout->setSpacing( 2 );
    GroupBox1Layout->setMargin( 2 );

    TextLabel2_2_2 = new QLabel( GroupBox1, "TextLabel2_2_2" );
    TextLabel2_2_2->setText( tr( "Noise" ) );

    GroupBox1Layout->addWidget( TextLabel2_2_2, 1, 0 );

    TextLabel3_2_2 = new QLabel( GroupBox1, "TextLabel3_2_2" );
    TextLabel3_2_2->setText( tr( "Quality" ) );

    GroupBox1Layout->addWidget( TextLabel3_2_2, 2, 0 );

    Noise_PB = new QProgressBar( GroupBox1, "Noise_PB" );
    Noise_PB->setProgress( 0 );

    GroupBox1Layout->addWidget( Noise_PB, 1, 2 );

    Quality_PB = new QProgressBar( GroupBox1, "Quality_PB" );
    Quality_PB->setProgress( 0 );

    GroupBox1Layout->addWidget( Quality_PB, 2, 2 );

    TextLabel1_2_2 = new QLabel( GroupBox1, "TextLabel1_2_2" );
    TextLabel1_2_2->setText( tr( "Signal" ) );

    GroupBox1Layout->addWidget( TextLabel1_2_2, 0, 0 );
    QSpacerItem* spacer_4 = new QSpacerItem( 16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    GroupBox1Layout->addItem( spacer_4, 0, 1 );

    Signal_PB = new QProgressBar( GroupBox1, "Signal_PB" );
    Signal_PB->setProgress( 0 );

    GroupBox1Layout->addWidget( Signal_PB, 0, 2 );
    tabLayout->addWidget( GroupBox1 );
    QSpacerItem* spacer_5 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    tabLayout->addItem( spacer_5 );
    tabWidget->insertTab( tab, tr( "State" ) );
    WLanGUILayout->addWidget( tabWidget );

    // signals and slots connections
    connect( wepEnabled, SIGNAL( toggled(bool) ), KeyButtonGroup, SLOT( setEnabled(bool) ) );
    connect( wepEnabled, SIGNAL( toggled(bool) ), NonEncButtonGroup, SLOT( setEnabled(bool) ) );
    connect( specifyAp, SIGNAL( toggled(bool) ), macEdit, SLOT( setEnabled(bool) ) );
    connect( specifyAp, SIGNAL( toggled(bool) ), macLabel, SLOT( setEnabled(bool) ) );
    connect( specifyChan, SIGNAL( toggled(bool) ), networkChannel, SLOT( setEnabled(bool) ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
WLanGUI::~WLanGUI()
{
    // no need to delete child widgets, Qt does it all for us
}

