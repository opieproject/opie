/****************************************************************************
** Form implementation generated from reading ui file 'ntpbase.ui'
**
** Created: Mon Oct 21 21:32:47 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "ntpbase.h"

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qlabel.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qtable.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a NtpBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
NtpBase::NtpBase( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "NtpBase" );
    resize( 320, 411 ); 
    setCaption( tr( "Network Time" ) );
    NtpBaseLayout = new QGridLayout( this ); 
    NtpBaseLayout->setSpacing( 2 );
    NtpBaseLayout->setMargin( 2 );

    TabWidgetMain = new QTabWidget( this, "TabWidgetMain" );

    tabMain = new QWidget( TabWidgetMain, "tabMain" );
    tabMainLayout = new QGridLayout( tabMain ); 
    tabMainLayout->setSpacing( 2 );
    tabMainLayout->setMargin( 2 );

    FrameSystemTime = new QFrame( tabMain, "FrameSystemTime" );
    FrameSystemTime->setFrameShape( QFrame::StyledPanel );
    FrameSystemTime->setFrameShadow( QFrame::Raised );

    tabMainLayout->addWidget( FrameSystemTime, 0, 0 );

    tabNtp = new QWidget( TabWidgetMain, "tabNtp" );
    tabNtpLayout = new QGridLayout( tabNtp ); 
    tabNtpLayout->setSpacing( 2 );
    tabNtpLayout->setMargin( 1 );

    runNtp = new QPushButton( tabNtp, "runNtp" );
    runNtp->setText( tr( "Get time from network" ) );

    tabNtpLayout->addWidget( runNtp, 1, 0 );

    FrameNtp = new QFrame( tabNtp, "FrameNtp" );
    FrameNtp->setFrameShape( QFrame::StyledPanel );
    FrameNtp->setFrameShadow( QFrame::Raised );
    FrameNtpLayout = new QGridLayout( FrameNtp ); 
    FrameNtpLayout->setSpacing( 2 );
    FrameNtpLayout->setMargin( 2 );

    Layout4 = new QVBoxLayout; 
    Layout4->setSpacing( 6 );
    Layout4->setMargin( 0 );

    TextLabel1 = new QLabel( FrameNtp, "TextLabel1" );
    TextLabel1->setAutoMask( FALSE );
    TextLabel1->setText( tr( "Start Time:" ) );
    Layout4->addWidget( TextLabel1 );

    TextLabelStartTime = new QLabel( FrameNtp, "TextLabelStartTime" );
    TextLabelStartTime->setText( tr( "nan" ) );
    Layout4->addWidget( TextLabelStartTime );

    TextLabel3 = new QLabel( FrameNtp, "TextLabel3" );
    TextLabel3->setText( tr( "Time Shift:" ) );
    Layout4->addWidget( TextLabel3 );

    TextLabelTimeShift = new QLabel( FrameNtp, "TextLabelTimeShift" );
    TextLabelTimeShift->setText( tr( "nan" ) );
    Layout4->addWidget( TextLabelTimeShift );

    TextLabel5 = new QLabel( FrameNtp, "TextLabel5" );
    TextLabel5->setText( tr( "New Time:" ) );
    Layout4->addWidget( TextLabel5 );

    TextLabelNewTime = new QLabel( FrameNtp, "TextLabelNewTime" );
    TextLabelNewTime->setText( tr( "nan" ) );
    Layout4->addWidget( TextLabelNewTime );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout4->addItem( spacer );

    FrameNtpLayout->addLayout( Layout4, 0, 0 );

    MultiLineEditntpOutPut = new QMultiLineEdit( FrameNtp, "MultiLineEditntpOutPut" );
    QFont MultiLineEditntpOutPut_font(  MultiLineEditntpOutPut->font() );
    MultiLineEditntpOutPut_font.setPointSize( 7 );
    MultiLineEditntpOutPut->setFont( MultiLineEditntpOutPut_font ); 
    MultiLineEditntpOutPut->setWordWrap( QMultiLineEdit::WidgetWidth );
    MultiLineEditntpOutPut->setText( tr( "" ) );

    FrameNtpLayout->addWidget( MultiLineEditntpOutPut, 1, 0 );

    tabNtpLayout->addWidget( FrameNtp, 0, 0 );

    tabPredict = new QWidget( TabWidgetMain, "tabPredict" );
    tabPredictLayout = new QGridLayout( tabPredict ); 
    tabPredictLayout->setSpacing( 6 );
    tabPredictLayout->setMargin( 5 );

    TableLookups = new QTable( tabPredict, "TableLookups" );
    QFont TableLookups_font(  TableLookups->font() );
    TableLookups_font.setPointSize( 8 );
    TableLookups->setFont( TableLookups_font ); 
    TableLookups->setNumRows( 2 );
    TableLookups->setNumCols( 2 );

    tabPredictLayout->addWidget( TableLookups, 0, 0 );

    Layout9 = new QGridLayout; 
    Layout9->setSpacing( 6 );
    Layout9->setMargin( 0 );

    TextLabelShift = new QLabel( tabPredict, "TextLabelShift" );
    TextLabelShift->setText( tr( "nan" ) );

    Layout9->addWidget( TextLabelShift, 0, 1 );

    TextLabel4 = new QLabel( tabPredict, "TextLabel4" );
    TextLabel4->setText( tr( "Esimated Shift:" ) );

    Layout9->addWidget( TextLabel4, 1, 0 );

    TextLabelEstimatedShift = new QLabel( tabPredict, "TextLabelEstimatedShift" );
    TextLabelEstimatedShift->setText( tr( "nan" ) );

    Layout9->addWidget( TextLabelEstimatedShift, 1, 1 );

    TextLabel3_2 = new QLabel( tabPredict, "TextLabel3_2" );
    TextLabel3_2->setText( tr( "Predicted Time:" ) );

    Layout9->addWidget( TextLabel3_2, 2, 0 );

    Mean_shift_label = new QLabel( tabPredict, "Mean_shift_label" );
    Mean_shift_label->setText( tr( "Mean shift:" ) );

    Layout9->addWidget( Mean_shift_label, 0, 0 );

    TextLabelPredTime = new QLabel( tabPredict, "TextLabelPredTime" );
    TextLabelPredTime->setText( tr( "nan" ) );

    Layout9->addWidget( TextLabelPredTime, 2, 1 );

    tabPredictLayout->addLayout( Layout9, 1, 0 );

    Layout11 = new QHBoxLayout; 
    Layout11->setSpacing( 6 );
    Layout11->setMargin( 0 );

    PushButtonSetPredTime = new QPushButton( tabPredict, "PushButtonSetPredTime" );
    PushButtonSetPredTime->setText( tr( "Set predicted time" ) );
    Layout11->addWidget( PushButtonSetPredTime );

    PushButtonPredict = new QPushButton( tabPredict, "PushButtonPredict" );
    PushButtonPredict->setText( tr( "Predict time" ) );
    Layout11->addWidget( PushButtonPredict );

    tabPredictLayout->addLayout( Layout11, 2, 0 );

    TabSettings = new QWidget( TabWidgetMain, "TabSettings" );
    TabSettingsLayout = new QGridLayout( TabSettings ); 
    TabSettingsLayout->setSpacing( 2 );
    TabSettingsLayout->setMargin( 2 );

    FrameSettings = new QFrame( TabSettings, "FrameSettings" );
    FrameSettings->setFrameShape( QFrame::StyledPanel );
    FrameSettings->setFrameShadow( QFrame::Raised );
    FrameSettingsLayout = new QVBoxLayout( FrameSettings ); 
    FrameSettingsLayout->setSpacing( 6 );
    FrameSettingsLayout->setMargin( 11 );

    Layout6 = new QGridLayout; 
    Layout6->setSpacing( 6 );
    Layout6->setMargin( 0 );

    TextLabel7_2 = new QLabel( FrameSettings, "TextLabel7_2" );
    TextLabel7_2->setText( tr( "Use" ) );

    Layout6->addWidget( TextLabel7_2, 0, 0 );

    TextLabel2_2 = new QLabel( FrameSettings, "TextLabel2_2" );
    TextLabel2_2->setText( tr( "as" ) );

    Layout6->addWidget( TextLabel2_2, 0, 2 );

    TextLabel1_3 = new QLabel( FrameSettings, "TextLabel1_3" );
    TextLabel1_3->setText( tr( "NTP server to get the time from the network." ) );
    TextLabel1_3->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter | QLabel::AlignLeft ) );

    Layout6->addMultiCellWidget( TextLabel1_3, 1, 1, 0, 1 );

    ComboNtpSrv = new QComboBox( FALSE, FrameSettings, "ComboNtpSrv" );
    ComboNtpSrv->setEditable( TRUE );

    Layout6->addWidget( ComboNtpSrv, 0, 1 );
    FrameSettingsLayout->addLayout( Layout6 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum );
    FrameSettingsLayout->addItem( spacer_2 );

    CheckBoxAdvSettings = new QCheckBox( FrameSettings, "CheckBoxAdvSettings" );
    CheckBoxAdvSettings->setText( tr( "Advanced features" ) );
    FrameSettingsLayout->addWidget( CheckBoxAdvSettings );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    FrameSettingsLayout->addItem( spacer_3 );

    Line1 = new QFrame( FrameSettings, "Line1" );
    Line1->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    FrameSettingsLayout->addWidget( Line1 );

    Layout7_2 = new QGridLayout; 
    Layout7_2->setSpacing( 6 );
    Layout7_2->setMargin( 0 );

    TextLabel1_2_2 = new QLabel( FrameSettings, "TextLabel1_2_2" );
    TextLabel1_2_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, TextLabel1_2_2->sizePolicy().hasHeightForWidth() ) );
    TextLabel1_2_2->setText( tr( "Wait for " ) );

    Layout7_2->addWidget( TextLabel1_2_2, 0, 0 );

    TextLabel2_3 = new QLabel( FrameSettings, "TextLabel2_3" );
    TextLabel2_3->setText( tr( "minutes until" ) );

    Layout7_2->addWidget( TextLabel2_3, 0, 2 );

    TextLabel3_3_2 = new QLabel( FrameSettings, "TextLabel3_3_2" );
    TextLabel3_3_2->setText( tr( "NTP tries to syncronises the clock with the network." ) );
    TextLabel3_3_2->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter | QLabel::AlignLeft ) );

    Layout7_2->addMultiCellWidget( TextLabel3_3_2, 1, 1, 0, 2 );

    SpinBoxNtpDelay = new QSpinBox( FrameSettings, "SpinBoxNtpDelay" );
    SpinBoxNtpDelay->setWrapping( TRUE );
    SpinBoxNtpDelay->setMaxValue( 9999999 );
    SpinBoxNtpDelay->setMinValue( 1 );
    SpinBoxNtpDelay->setValue( 1440 );

    Layout7_2->addWidget( SpinBoxNtpDelay, 0, 1 );
    FrameSettingsLayout->addLayout( Layout7_2 );

    Layout7 = new QGridLayout; 
    Layout7->setSpacing( 6 );
    Layout7->setMargin( 0 );

    TextLabel1_2 = new QLabel( FrameSettings, "TextLabel1_2" );
    TextLabel1_2->setText( tr( "Insure a delay of" ) );

    Layout7->addWidget( TextLabel1_2, 0, 0 );

    TextLabel2 = new QLabel( FrameSettings, "TextLabel2" );
    TextLabel2->setText( tr( "minutes until" ) );

    Layout7->addWidget( TextLabel2, 0, 2 );

    TextLabel3_3 = new QLabel( FrameSettings, "TextLabel3_3" );
    TextLabel3_3->setText( tr( "a new NTP lookup will be used to predict the time." ) );
    TextLabel3_3->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter | QLabel::AlignLeft ) );

    Layout7->addMultiCellWidget( TextLabel3_3, 1, 1, 0, 2 );

    SpinBoxMinLookupDelay = new QSpinBox( FrameSettings, "SpinBoxMinLookupDelay" );
    SpinBoxMinLookupDelay->setWrapping( TRUE );
    SpinBoxMinLookupDelay->setMaxValue( 9999999 );
    SpinBoxMinLookupDelay->setMinValue( 42 );
    SpinBoxMinLookupDelay->setValue( 720 );

    Layout7->addWidget( SpinBoxMinLookupDelay, 0, 1 );
    FrameSettingsLayout->addLayout( Layout7 );

    TabSettingsLayout->addWidget( FrameSettings, 0, 0 );

    tabManualSetTime = new QWidget( TabWidgetMain, "tabManualSetTime" );
    tabManualSetTimeLayout = new QVBoxLayout( tabManualSetTime ); 
    tabManualSetTimeLayout->setSpacing( 2 );
    tabManualSetTimeLayout->setMargin( 2 );

    FrameSetTime = new QFrame( tabManualSetTime, "FrameSetTime" );
    FrameSetTime->setFrameShape( QFrame::StyledPanel );
    FrameSetTime->setFrameShadow( QFrame::Raised );
    tabManualSetTimeLayout->addWidget( FrameSetTime );

    PushButtonSetManualTime = new QPushButton( tabManualSetTime, "PushButtonSetManualTime" );
    PushButtonSetManualTime->setText( tr( "Set time" ) );
    tabManualSetTimeLayout->addWidget( PushButtonSetManualTime );


    TabWidgetMain->insertTab( tabMain, tr( "Main" ) );
    TabWidgetMain->insertTab( tabManualSetTime, tr( "Manual" ) );
    TabWidgetMain->insertTab( TabSettings, tr( "Settings" ) );
    TabWidgetMain->insertTab( tabPredict, tr( "Predict" ) );
    TabWidgetMain->insertTab( tabNtp, tr( "NTP" ) );

    NtpBaseLayout->addWidget( TabWidgetMain, 0, 0 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
NtpBase::~NtpBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*  
 *  Main event handler. Reimplemented to handle application
 *  font changes
 */
bool NtpBase::event( QEvent* ev )
{
    bool ret = QWidget::event( ev ); 
    if ( ev->type() == QEvent::ApplicationFontChange ) {
	QFont MultiLineEditntpOutPut_font(  MultiLineEditntpOutPut->font() );
	MultiLineEditntpOutPut_font.setPointSize( 7 );
	MultiLineEditntpOutPut->setFont( MultiLineEditntpOutPut_font ); 
	QFont TableLookups_font(  TableLookups->font() );
	TableLookups_font.setPointSize( 8 );
	TableLookups->setFont( TableLookups_font ); 
    }
    return ret;
}

