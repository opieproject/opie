/****************************************************************************
** Form implementation generated from reading ui file 'datebookweeklstheader.ui'
**
** Created: Mon Mar 10 20:50:34 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "datebookweeklstheader.h"

#include <qlabel.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include <qpe/resource.h>

static const char* const image0_data[] = { 
"14 14 19 1",
". c None",
"# c #000000",
"c c #202420",
"a c #202429",
"f c #525052",
"o c #52595a",
"m c #5a5d5a",
"l c #cdcacd",
"k c #cdcecd",
"q c #d5d6d5",
"i c #ded6de",
"j c #e6dee6",
"p c #eeeaee",
"h c #f6eef6",
"n c #f6f2f6",
"e c #f6f6f6",
"g c #fff6ff",
"d c #fffaff",
"b c #ffffff",
"..............",
".......##.....",
"......ab#.....",
".....cbb##....",
"....abbd####..",
"...cbbbbbbe#..",
"..fbbbbgdhi##.",
"...abbejkll##.",
"...maenk#####.",
"....oapk####..",
".....oaq##....",
"......o###....",
"..............",
".............."};

static const char* const image1_data[] = { 
"14 14 22 1",
". c None",
"# c #000000",
"c c #202020",
"g c #202420",
"q c #414041",
"k c #4a484a",
"e c #5a5d62",
"d c #5a6162",
"a c #62696a",
"p c #cdcacd",
"s c #d5ced5",
"o c #d5d2d5",
"j c #ded6de",
"r c #dedade",
"l c #e6e2e6",
"m c #eeeaee",
"h c #f6eef6",
"i c #f6f2f6",
"t c #f6f6f6",
"f c #fff6ff",
"n c #fffaff",
"b c #ffffff",
"..............",
".....##a......",
".....#bcd.....",
".....#bbce....",
"..####bbfge...",
"..#bbbbbfhce..",
"..#bbbbbijjk..",
"..#flmniopcq..",
"..####nrsgq#..",
"....##tpcq#...",
".....#hgq#....",
".....##q#.....",
"..............",
".............."};


/* 
 *  Constructs a DateBookWeekLstHeaderBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
DateBookWeekLstHeaderBase::DateBookWeekLstHeaderBase( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    QPixmap image0( ( const char** ) image0_data );
    QPixmap image1( ( const char** ) image1_data );
    if ( !name )
	setName( "DateBookWeekLstHeaderBase" );
    resize( 447, 45 ); 
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, sizePolicy().hasHeightForWidth() ) );
    DateBookWeekLstHeaderBaseLayout = new QHBoxLayout( this ); 
    DateBookWeekLstHeaderBaseLayout->setSpacing( 6 );
    DateBookWeekLstHeaderBaseLayout->setMargin( 11 );

    back = new QToolButton( this, "back" );
    back->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, back->sizePolicy().hasHeightForWidth() ) );
    back->setText( tr( "" ) );
    back->setPixmap( image0 );
    back->setPixmap(  Resource::loadPixmap( "back" ) );
    back->setToggleButton( FALSE );
    back->setAutoRepeat( TRUE );
    back->setAutoRaise( TRUE );
    back->setToggleButton( FALSE );
    QToolTip::add(  back, tr( "" ) );
    DateBookWeekLstHeaderBaseLayout->addWidget( back );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    DateBookWeekLstHeaderBaseLayout->addItem( spacer );

    labelWeek = new QToolButton( this, "labelWeek" );
    labelWeek->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, labelWeek->sizePolicy().hasHeightForWidth() ) );
    QFont labelWeek_font(  labelWeek->font() );
    labelWeek_font.setBold( TRUE );
    labelWeek->setFont( labelWeek_font ); 
    labelWeek->setText( tr( "W: 00,00" ) );
    labelWeek->setToggleButton( FALSE );
    labelWeek->setAutoRaise( FALSE );
    labelWeek->setToggleButton( FALSE );
    QToolTip::add(  labelWeek, tr( "" ) );
    DateBookWeekLstHeaderBaseLayout->addWidget( labelWeek );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    DateBookWeekLstHeaderBaseLayout->addItem( spacer_2 );

    dbl = new QToolButton( this, "dbl" );
    dbl->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, dbl->sizePolicy().hasHeightForWidth() ) );
    QFont dbl_font(  dbl->font() );
    dbl_font.setBold( TRUE );
    dbl->setFont( dbl_font ); 
    dbl->setText( tr( "2" ) );
    dbl->setToggleButton( TRUE );
    dbl->setAutoRaise( FALSE );
    dbl->setToggleButton( TRUE );
    QToolTip::add(  dbl, tr( "" ) );
    DateBookWeekLstHeaderBaseLayout->addWidget( dbl );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    DateBookWeekLstHeaderBaseLayout->addItem( spacer_3 );

    labelDate = new QLabel( this, "labelDate" );
    labelDate->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)7, labelDate->sizePolicy().hasHeightForWidth() ) );
    QFont labelDate_font(  labelDate->font() );
    labelDate_font.setBold( TRUE );
    labelDate->setFont( labelDate_font ); 
    labelDate->setText( tr( "00 Jan-00 Jan" ) );
    DateBookWeekLstHeaderBaseLayout->addWidget( labelDate );
    QSpacerItem* spacer_4 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    DateBookWeekLstHeaderBaseLayout->addItem( spacer_4 );

    forward = new QToolButton( this, "forward" );
    forward->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, forward->sizePolicy().hasHeightForWidth() ) );
    forward->setText( tr( "" ) );
    forward->setPixmap( image1 );
    forward->setPixmap(  Resource::loadPixmap( "forward" ) );
    forward->setAutoRepeat( TRUE );
    forward->setAutoRaise( TRUE );
    DateBookWeekLstHeaderBaseLayout->addWidget( forward );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DateBookWeekLstHeaderBase::~DateBookWeekLstHeaderBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*  
 *  Main event handler. Reimplemented to handle application
 *  font changes
 */
bool DateBookWeekLstHeaderBase::event( QEvent* ev )
{
    bool ret = QWidget::event( ev ); 
    if ( ev->type() == QEvent::ApplicationFontChange ) {
	QFont labelWeek_font(  labelWeek->font() );
	labelWeek_font.setBold( TRUE );
	labelWeek->setFont( labelWeek_font ); 
	QFont dbl_font(  dbl->font() );
	dbl_font.setBold( TRUE );
	dbl->setFont( dbl_font ); 
	QFont labelDate_font(  labelDate->font() );
	labelDate_font.setBold( TRUE );
	labelDate->setFont( labelDate_font ); 
    }
    return ret;
}

