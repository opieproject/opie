/****************************************************************************
** Form implementation generated from reading ui file 'dateentry.ui'
**
** Created: Mon Mar 10 20:50:24 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "dateentry.h"

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpe/categoryselect.h>
#include <qpe/tzselect.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qtoolbutton.h>
#include "timepicker.h"
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a DateEntryBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
DateEntryBase::DateEntryBase( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "DateEntryBase" );
    resize( 242, 339 ); 
    setCaption( tr( "New Event" ) );
    DateEntryBaseLayout = new QGridLayout( this ); 
    DateEntryBaseLayout->setSpacing( 0 );
    DateEntryBaseLayout->setMargin( 2 );

    TextLabel2 = new QLabel( this, "TextLabel2" );
    TextLabel2->setFrameShape( QLabel::MShape );
    TextLabel2->setFrameShadow( QLabel::MShadow );
    TextLabel2->setText( tr( "Location" ) );

    DateEntryBaseLayout->addWidget( TextLabel2, 1, 0 );

    TextLabel2_2 = new QLabel( this, "TextLabel2_2" );
    TextLabel2_2->setText( tr( "Category" ) );

    DateEntryBaseLayout->addWidget( TextLabel2_2, 2, 0 );

    comboDescription = new QComboBox( FALSE, this, "comboDescription" );
    comboDescription->insertItem( tr( "" ) );
    comboDescription->insertItem( tr( "Meeting" ) );
    comboDescription->insertItem( tr( "Lunch" ) );
    comboDescription->insertItem( tr( "Dinner" ) );
    comboDescription->insertItem( tr( "Travel" ) );
    comboDescription->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, comboDescription->sizePolicy().hasHeightForWidth() ) );
    comboDescription->setEditable( TRUE );
    comboDescription->setCurrentItem( 0 );
    comboDescription->setDuplicatesEnabled( FALSE );

    DateEntryBaseLayout->addMultiCellWidget( comboDescription, 0, 0, 1, 3 );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setText( tr( "Description " ) );

    DateEntryBaseLayout->addWidget( TextLabel1, 0, 0 );

    comboLocation = new QComboBox( FALSE, this, "comboLocation" );
    comboLocation->insertItem( tr( "" ) );
    comboLocation->insertItem( tr( "Office" ) );
    comboLocation->insertItem( tr( "Home" ) );
    comboLocation->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, comboLocation->sizePolicy().hasHeightForWidth() ) );
    comboLocation->setEditable( TRUE );
    comboLocation->setCurrentItem( 0 );
    comboLocation->setDuplicatesEnabled( FALSE );

    DateEntryBaseLayout->addMultiCellWidget( comboLocation, 1, 1, 1, 3 );

    comboCategory = new CategorySelect( this, "comboCategory" );

    DateEntryBaseLayout->addMultiCellWidget( comboCategory, 2, 2, 1, 3 );

    TextLabel3 = new QLabel( this, "TextLabel3" );
    TextLabel3->setText( tr( "Start - End " ) );

    DateEntryBaseLayout->addMultiCellWidget( TextLabel3, 3, 4, 0, 0 );

    buttonStart = new QPushButton( this, "buttonStart" );
    buttonStart->setText( tr( "Jan 02 00" ) );
    buttonStart->setMinimumSize( QSize( 70, 0 ) );
    buttonStart->setMaximumSize( QSize( 70, 32767 ) );

    DateEntryBaseLayout->addWidget( buttonStart, 3, 1 );

    comboStart = new QLineEdit( this, "comboStart" );
    comboStart->setMinimumSize( QSize( 70, 0 ) );
    comboStart->setMaximumSize( QSize( 70, 32767 ) );
    comboStart->setAlignment( int( QLineEdit::AlignHCenter ) );

    DateEntryBaseLayout->addWidget( comboStart, 4, 1 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    DateEntryBaseLayout->addMultiCell( spacer, 3, 4, 2, 2 );

    timePickerStart = new TimePicker( this, "timePickerStart" );

    DateEntryBaseLayout->addMultiCellWidget( timePickerStart, 5, 5, 1, 3 );

    buttonEnd = new QPushButton( this, "buttonEnd" );
    buttonEnd->setText( tr( "Jan 02 00" ) );
    buttonEnd->setMinimumSize( QSize( 70, 0 ) );
    buttonEnd->setMaximumSize( QSize( 70, 32767 ) );

    DateEntryBaseLayout->addWidget( buttonEnd, 3, 3 );

    comboEnd = new QLineEdit( this, "comboEnd" );
    comboEnd->setMinimumSize( QSize( 70, 0 ) );
    comboEnd->setMaximumSize( QSize( 70, 32767 ) );
    comboEnd->setAlignment( int( QLineEdit::AlignHCenter ) );

    DateEntryBaseLayout->addWidget( comboEnd, 4, 3 );

    TimePickerLabel = new QLabel( this, "TimePickerLabel" );
    TimePickerLabel->setText( tr( "Start time" ) );

    DateEntryBaseLayout->addWidget( TimePickerLabel, 5, 0 );

    checkAllDay = new QCheckBox( this, "checkAllDay" );
    checkAllDay->setText( tr( "All day" ) );

    DateEntryBaseLayout->addWidget( checkAllDay, 6, 0 );

    TextLabel3_2_2 = new QLabel( this, "TextLabel3_2_2" );
    TextLabel3_2_2->setText( tr( "Time zone" ) );

    DateEntryBaseLayout->addWidget( TextLabel3_2_2, 7, 0 );

    timezone = new TimeZoneSelector( this, "timezone" );

    DateEntryBaseLayout->addMultiCellWidget( timezone, 7, 7, 1, 3 );

    checkAlarm = new QCheckBox( this, "checkAlarm" );
    checkAlarm->setEnabled( TRUE );
    checkAlarm->setAutoMask( FALSE );
    checkAlarm->setText( tr( "&Alarm" ) );
    checkAlarm->setChecked( FALSE );

    DateEntryBaseLayout->addWidget( checkAlarm, 8, 0 );

    spinAlarm = new QSpinBox( this, "spinAlarm" );
    spinAlarm->setEnabled( FALSE );
    spinAlarm->setSuffix( tr( " minutes" ) );
    spinAlarm->setMaxValue( 180 );
    spinAlarm->setMinValue( 0 );
    spinAlarm->setLineStep( 5 );
    spinAlarm->setValue( 5 );

    DateEntryBaseLayout->addMultiCellWidget( spinAlarm, 8, 8, 1, 2 );

    comboSound = new QComboBox( FALSE, this, "comboSound" );
    comboSound->insertItem( tr( "Silent" ) );
    comboSound->insertItem( tr( "Loud" ) );
    comboSound->setEnabled( FALSE );

    DateEntryBaseLayout->addWidget( comboSound, 8, 3 );

    lblRepeat = new QLabel( this, "lblRepeat" );
    lblRepeat->setText( tr( "Repeat" ) );

    DateEntryBaseLayout->addWidget( lblRepeat, 9, 0 );

    cmdRepeat = new QToolButton( this, "cmdRepeat" );
    cmdRepeat->setFocusPolicy( QToolButton::TabFocus );
    cmdRepeat->setText( tr( "No Repeat..." ) );
    cmdRepeat->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, cmdRepeat->sizePolicy().hasHeightForWidth() ) );

    DateEntryBaseLayout->addMultiCellWidget( cmdRepeat, 9, 9, 1, 3 );

    editNote = new QToolButton( this, "editNote" );
    editNote->setText( tr( "Note..." ) );

    DateEntryBaseLayout->addMultiCellWidget( editNote, 10, 10, 0, 3 );

    // signals and slots connections
    connect( checkAlarm, SIGNAL( toggled(bool) ), spinAlarm, SLOT( setEnabled(bool) ) );
    connect( comboEnd, SIGNAL( textChanged(const QString&) ), this, SLOT( endTimeChanged( const QString & ) ) );
    connect( cmdRepeat, SIGNAL( clicked() ), this, SLOT( slotRepeat() ) );
    connect( comboStart, SIGNAL( textChanged(const QString &) ), this, SLOT( startTimeEdited( const QString & ) ) );
    connect( checkAllDay, SIGNAL( toggled(bool) ), comboEnd, SLOT( setDisabled(bool) ) );
    connect( checkAllDay, SIGNAL( toggled(bool) ), timePickerStart, SLOT( setDisabled(bool) ) );
    connect( checkAlarm, SIGNAL( toggled(bool) ), comboSound, SLOT( setEnabled(bool) ) );
    connect( checkAllDay, SIGNAL( toggled(bool) ), comboStart, SLOT( setDisabled(bool) ) );
    connect( checkAllDay, SIGNAL( toggled(bool) ), buttonEnd, SLOT( setDisabled(bool) ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DateEntryBase::~DateEntryBase()
{
    // no need to delete child widgets, Qt does it all for us
}

void DateEntryBase::endDateChanged( const QString & )
{
    qWarning( "DateEntryBase::endDateChanged( const QString & ): Not implemented yet!" );
}

void DateEntryBase::endDateChanged( int, int, int )
{
    qWarning( "DateEntryBase::endDateChanged( int, int, int ): Not implemented yet!" );
}

void DateEntryBase::endTimeChanged( const QString & )
{
    qWarning( "DateEntryBase::endTimeChanged( const QString & ): Not implemented yet!" );
}

void DateEntryBase::slotRepeat()
{
    qWarning( "DateEntryBase::slotRepeat(): Not implemented yet!" );
}

void DateEntryBase::slotWait( int )
{
    qWarning( "DateEntryBase::slotWait( int ): Not implemented yet!" );
}

void DateEntryBase::startDateChanged( const QString & )
{
    qWarning( "DateEntryBase::startDateChanged( const QString & ): Not implemented yet!" );
}

void DateEntryBase::startDateChanged(int, int, int)
{
    qWarning( "DateEntryBase::startDateChanged(int, int, int): Not implemented yet!" );
}

void DateEntryBase::startTimeEdited( const QString & )
{
    qWarning( "DateEntryBase::startTimeEdited( const QString & ): Not implemented yet!" );
}

void DateEntryBase::typeChanged( const QString & )
{
    qWarning( "DateEntryBase::typeChanged( const QString & ): Not implemented yet!" );
}

void DateEntryBase::tzexecute(void)
{
    qWarning( "DateEntryBase::tzexecute(void): Not implemented yet!" );
}

