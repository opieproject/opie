/****************************************************************************
** Form implementation generated from reading ui file 'settingsdialogbase.ui'
**
** Created: Sun Nov 3 07:29:03 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "settingsdialogbase.h"

#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a SettingsDialogBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
SettingsDialogBase::SettingsDialogBase( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
        setName( "SettingsDialogBase" );
    resize( 246, 201 ); 
    setCaption( tr( "Preferences" ) );
    SettingsDialogBaseLayout = new QVBoxLayout( this ); 
    SettingsDialogBaseLayout->setSpacing( 6 );
    SettingsDialogBaseLayout->setMargin( 6 );

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setTitle( tr( "Slide Show" ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 0 );
    GroupBox1->layout()->setMargin( 0 );
    GroupBox1Layout = new QVBoxLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );
    GroupBox1Layout->setSpacing( 6 );
    GroupBox1Layout->setMargin( 11 );

    Layout3 = new QGridLayout; 
    Layout3->setSpacing( 6 );
    Layout3->setMargin( 0 );

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setText( tr( "Delay between pictures" ) );

    Layout3->addMultiCellWidget( TextLabel1, 0, 0, 0, 1 );

    delaySlider = new QSlider( GroupBox1, "delaySlider" );
    delaySlider->setMinValue( 2 );
    delaySlider->setMaxValue( 60 );
    delaySlider->setLineStep( 2 );
    delaySlider->setOrientation( QSlider::Horizontal );
    delaySlider->setTickmarks( QSlider::Right );
    delaySlider->setTickInterval( 10);

    Layout3->addWidget( delaySlider, 1, 0 );

    delayText = new QLabel( GroupBox1, "delayText" );
    delayText->setMinimumSize( QSize( 25, 0 ) );
    delayText->setText( tr( "s" ) );
    delayText->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    Layout3->addWidget( delayText, 1, 1 );
    GroupBox1Layout->addLayout( Layout3 );

    repeatCheck = new QCheckBox( GroupBox1, "repeatCheck" );
    repeatCheck->setText( tr( "Repeat slideshow" ) );
    GroupBox1Layout->addWidget( repeatCheck );

    reverseCheck = new QCheckBox( GroupBox1, "reverseCheck" );
    reverseCheck->setText( tr( "Show pictures in reverse" ) );
    GroupBox1Layout->addWidget( reverseCheck );
    SettingsDialogBaseLayout->addWidget( GroupBox1 );

    rotateCheck = new QCheckBox( this, "rotateCheck" );
    rotateCheck->setText( tr( "Load pictures rotated 90 degrees" ) );
    SettingsDialogBaseLayout->addWidget( rotateCheck );

    fastLoadCheck = new QCheckBox( this, "fastLoadCheck" );
    fastLoadCheck->setText( tr( "Fast load pictures" ) );
    SettingsDialogBaseLayout->addWidget( fastLoadCheck );
    fastLoadCheck->hide(); //FIXME
//    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding );
//    layout->addItem( spacer, 10, 0 );

    
}

/*  
 *  Destroys the object and frees any allocated resources
 */
SettingsDialogBase::~SettingsDialogBase()
{
    // no need to delete child widgets, Qt does it all for us
}

