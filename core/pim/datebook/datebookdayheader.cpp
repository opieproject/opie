/****************************************************************************
** Form implementation generated from reading ui file 'datebookdayheader.ui'
**
** Created: Mon Mar 24 10:42:07 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "datebookdayheader.h"

#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>


/* 
 *  Constructs a DateBookDayHeaderBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
DateBookDayHeaderBase::DateBookDayHeaderBase( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "DateBookDayHeaderBase" );
    resize( 249, 26 ); 
    DateBookDayHeaderBaseLayout = new QHBoxLayout( this ); 
    DateBookDayHeaderBaseLayout->setSpacing( 0 );
    DateBookDayHeaderBaseLayout->setMargin( 0 );

    backweek = new QToolButton( this, "backweek" );
    backweek->setText( tr( "" ) );
    backweek->setToggleButton( FALSE );
    backweek->setAutoRepeat( TRUE );
    backweek->setAutoRaise( TRUE );
    backweek->setToggleButton( FALSE );
    QToolTip::add(  backweek, tr( "" ) );
    DateBookDayHeaderBaseLayout->addWidget( backweek );
    
	back = new QToolButton( this, "back" );
    back->setText( tr( "" ) );
    back->setToggleButton( FALSE );
    back->setAutoRepeat( TRUE );
    back->setAutoRaise( TRUE );
    back->setToggleButton( FALSE );
    QToolTip::add(  back, tr( "" ) );
    DateBookDayHeaderBaseLayout->addWidget( back );

    date = new QToolButton( this, "date" );
    date->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)1, date->sizePolicy().hasHeightForWidth() ) );
    QFont date_font(  date->font() );
    date_font.setBold( TRUE );
    date->setFont( date_font ); 
    date->setText( tr( "" ) );
    date->setAutoRepeat( FALSE );
    date->setAutoRaise( FALSE );
    DateBookDayHeaderBaseLayout->addWidget( date );

    grpDays = new QButtonGroup( this, "grpDays" );
    grpDays->setFrameShape( QButtonGroup::NoFrame );
    grpDays->setFrameShadow( QButtonGroup::Plain );
    grpDays->setTitle( tr( "" ) );
    grpDays->setExclusive( TRUE );
    grpDays->setColumnLayout(0, Qt::Vertical );
    grpDays->layout()->setSpacing( 0 );
    grpDays->layout()->setMargin( 0 );
    grpDaysLayout = new QHBoxLayout( grpDays->layout() );
    grpDaysLayout->setAlignment( Qt::AlignTop );
    grpDaysLayout->setSpacing( 1 );
    grpDaysLayout->setMargin( 0 );

    cmdDay1 = new QToolButton( grpDays, "cmdDay1" );
    cmdDay1->setText( tr( "M" ) );
    cmdDay1->setToggleButton( TRUE );
    cmdDay1->setAutoRaise( TRUE );
    cmdDay1->setToggleButton( TRUE );
    QToolTip::add(  cmdDay1, tr( "" ) );
    grpDaysLayout->addWidget( cmdDay1 );

    cmdDay2 = new QToolButton( grpDays, "cmdDay2" );
    cmdDay2->setText( tr( "T" ) );
    cmdDay2->setToggleButton( TRUE );
    cmdDay2->setAutoRaise( TRUE );
    cmdDay2->setToggleButton( TRUE );
    QToolTip::add(  cmdDay2, tr( "" ) );
    grpDaysLayout->addWidget( cmdDay2 );

    cmdDay3 = new QToolButton( grpDays, "cmdDay3" );
    cmdDay3->setText( tr( "W" ) );
    cmdDay3->setToggleButton( TRUE );
    cmdDay3->setAutoRaise( TRUE );
    cmdDay3->setToggleButton( TRUE );
    QToolTip::add(  cmdDay3, tr( "" ) );
    grpDaysLayout->addWidget( cmdDay3 );

    cmdDay4 = new QToolButton( grpDays, "cmdDay4" );
    cmdDay4->setText( tr( "T" ) );
    cmdDay4->setToggleButton( TRUE );
    cmdDay4->setAutoRepeat( FALSE );
    cmdDay4->setAutoRaise( TRUE );
    cmdDay4->setToggleButton( TRUE );
    QToolTip::add(  cmdDay4, tr( "" ) );
    grpDaysLayout->addWidget( cmdDay4 );

    cmdDay5 = new QToolButton( grpDays, "cmdDay5" );
    cmdDay5->setText( tr( "F" ) );
    cmdDay5->setToggleButton( TRUE );
    cmdDay5->setAutoRaise( TRUE );
    cmdDay5->setToggleButton( TRUE );
    QToolTip::add(  cmdDay5, tr( "" ) );
    grpDaysLayout->addWidget( cmdDay5 );

    cmdDay6 = new QToolButton( grpDays, "cmdDay6" );
    cmdDay6->setText( tr( "S" ) );
    cmdDay6->setToggleButton( TRUE );
    cmdDay6->setAutoRaise( TRUE );
    cmdDay6->setToggleButton( TRUE );
    QToolTip::add(  cmdDay6, tr( "" ) );
    grpDaysLayout->addWidget( cmdDay6 );

    cmdDay7 = new QToolButton( grpDays, "cmdDay7" );
    cmdDay7->setText( tr( "S" ) );
    cmdDay7->setToggleButton( TRUE );
    cmdDay7->setAutoRaise( TRUE );
    cmdDay7->setToggleButton( TRUE );
    QToolTip::add(  cmdDay7, tr( "" ) );
    grpDaysLayout->addWidget( cmdDay7 );
    DateBookDayHeaderBaseLayout->addWidget( grpDays );

    forward = new QToolButton( this, "forward" );
    forward->setText( tr( "" ) );
    forward->setAutoRepeat( TRUE );
    forward->setAutoRaise( TRUE );
    DateBookDayHeaderBaseLayout->addWidget( forward );

    forwardweek = new QToolButton( this, "forwardweek" );
    forwardweek->setText( tr( "" ) );
    forwardweek->setAutoRepeat( TRUE );
    forwardweek->setAutoRaise( TRUE );
    DateBookDayHeaderBaseLayout->addWidget( forwardweek );

    // signals and slots connections
    connect( forwardweek, SIGNAL( clicked() ), this, SLOT( goForwardWeek() ) );
    connect( forward, SIGNAL( clicked() ), this, SLOT( goForward() ) );
    connect( back, SIGNAL( clicked() ), this, SLOT( goBack() ) );
    connect( backweek, SIGNAL( clicked() ), this, SLOT( goBackWeek() ) );
    connect( grpDays, SIGNAL( clicked(int) ), this, SLOT( setDay( int ) ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DateBookDayHeaderBase::~DateBookDayHeaderBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*  
 *  Main event handler. Reimplemented to handle application
 *  font changes
 */
bool DateBookDayHeaderBase::event( QEvent* ev )
{
    bool ret = QWidget::event( ev ); 
    if ( ev->type() == QEvent::ApplicationFontChange ) {
	QFont date_font(  date->font() );
	date_font.setBold( TRUE );
	date->setFont( date_font ); 
    }
    return ret;
}

void DateBookDayHeaderBase::goBack()
{
    qWarning( "DateBookDayHeaderBase::goBack(): Not implemented yet!" );
}

void DateBookDayHeaderBase::goBackWeek()
{
    qWarning( "DateBookDayHeaderBase::goBackWeek(): Not implemented yet!" );
}

void DateBookDayHeaderBase::goForward()
{
    qWarning( "DateBookDayHeaderBase::goForward(): Not implemented yet!" );
}

void DateBookDayHeaderBase::goForwardWeek()
{
    qWarning( "DateBookDayHeaderBase::goForwardWeek(): Not implemented yet!" );
}

void DateBookDayHeaderBase::setDate( int, int, int )
{
    qWarning( "DateBookDayHeaderBase::setDate( int, int, int ): Not implemented yet!" );
}

void DateBookDayHeaderBase::setDay( int )
{
    qWarning( "DateBookDayHeaderBase::setDay( int ): Not implemented yet!" );
}

