

// changes by Maximilian Reiss <harlekin@handhelds.org>

#include "citytimebase.h"

#include <qlabel.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include "zonemap.h"
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>


/*
 *  Constructs a CityTimeBase which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 */
CityTimeBase::CityTimeBase( QWidget* parent,  const char* name, WFlags  )
    : QWidget( parent, name, WStyle_ContextHelp )
{
    if ( !name )
	setName( "CityTimeBase" );
    setCaption( tr( "City Time" ) );
    CityTimeBaseLayout = new QVBoxLayout( this );
    CityTimeBaseLayout->setSpacing( 3 );
    CityTimeBaseLayout->setMargin( 0 );

    frmMap = new ZoneMap( this, "frmMap" );
    frmMap->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, frmMap->sizePolicy().hasHeightForWidth() ) );
    CityTimeBaseLayout->addWidget( frmMap );

    buttonWidget = new QWidget( this );
    QWhatsThis::add( buttonWidget, tr( "Click on one of the set cities to replace it" ) );
    Layout2 = new QGridLayout( buttonWidget );
    Layout2->setSpacing( 3 );
    Layout2->setMargin( 4 );

    cmdCity7 = new QToolButton( buttonWidget, "cmdCity7" );
    cmdCity7->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, cmdCity7->sizePolicy().hasHeightForWidth() ) );
    QFont cmdCity7_font(  cmdCity7->font() );
    cmdCity7_font.setBold( TRUE );
    cmdCity7->setFont( cmdCity7_font );
    cmdCity7->setFocusPolicy( QToolButton::TabFocus );
    cmdCity7->setText( tr( "" ) );
    cmdCity7->setToggleButton( TRUE );
    cmdCity7->setToggleButton( TRUE );

    Layout2->addWidget( cmdCity7, 0, 2 );

    lblCTime9 = new QLabel( buttonWidget, "lblCTime9" );
    lblCTime9->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, lblCTime9->sizePolicy().hasHeightForWidth() ) );
    QFont lblCTime9_font(  lblCTime9->font() );
    lblCTime9_font.setPointSize( 10 );
    lblCTime9->setFont( lblCTime9_font );
    lblCTime9->setText( tr( "" ) );
    lblCTime9->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    Layout2->addWidget( lblCTime9, 2, 3 );

    cmdCity8 = new QToolButton( buttonWidget, "cmdCity8" );
    cmdCity8->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, cmdCity8->sizePolicy().hasHeightForWidth() ) );
    QFont cmdCity8_font(  cmdCity8->font() );
    cmdCity8_font.setBold( TRUE );
    cmdCity8->setFont( cmdCity8_font );
    cmdCity8->setFocusPolicy( QToolButton::TabFocus );
    cmdCity8->setText( tr( "" ) );
    cmdCity8->setToggleButton( TRUE );
    cmdCity8->setToggleButton( TRUE );

    Layout2->addWidget( cmdCity8, 1, 2 );

    cmdCity2 = new QToolButton( buttonWidget, "cmdCity2" );
    QFont cmdCity2_font(  cmdCity2->font() );
    cmdCity2_font.setBold( TRUE );
    cmdCity2->setFont( cmdCity2_font );
    cmdCity2->setFocusPolicy( QToolButton::TabFocus );
    cmdCity2->setText( tr( "" ) );
    cmdCity2->setToggleButton( TRUE );
    cmdCity2->setToggleButton( TRUE );

    Layout2->addWidget( cmdCity2, 1, 0 );

    lblCTime6 = new QLabel( buttonWidget, "lblCTime6" );
    QFont lblCTime6_font(  lblCTime6->font() );
    lblCTime6_font.setPointSize( 10 );
    lblCTime6->setFont( lblCTime6_font );
    lblCTime6->setText( tr( "" ) );
    lblCTime6->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    Layout2->addWidget( lblCTime6, 5, 1 );

    cmdCity6 = new QToolButton( buttonWidget, "cmdCity6" );
    cmdCity6->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, cmdCity6->sizePolicy().hasHeightForWidth() ) );
    QFont cmdCity6_font(  cmdCity6->font() );
    cmdCity6_font.setBold( TRUE );
    cmdCity6->setFont( cmdCity6_font );
    cmdCity6->setFocusPolicy( QToolButton::TabFocus );
    cmdCity6->setText( tr( "" ) );
    cmdCity6->setToggleButton( TRUE );
    cmdCity6->setToggleButton( TRUE );

    Layout2->addWidget( cmdCity6, 5, 0 );

    cmdCity4 = new QToolButton( buttonWidget, "cmdCity4" );
    cmdCity4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, cmdCity4->sizePolicy().hasHeightForWidth() ) );
    QFont cmdCity4_font(  cmdCity4->font() );
    cmdCity4_font.setBold( TRUE );
    cmdCity4->setFont( cmdCity4_font );
    cmdCity4->setFocusPolicy( QToolButton::TabFocus );
    cmdCity4->setText( tr( "" ) );
    cmdCity4->setToggleButton( TRUE );
    cmdCity4->setToggleButton( TRUE );

    Layout2->addWidget( cmdCity4, 3, 0 );

    lblCTime5 = new QLabel( buttonWidget, "lblCTime5" );
    QFont lblCTime5_font(  lblCTime5->font() );
    lblCTime5_font.setPointSize( 10 );
    lblCTime5->setFont( lblCTime5_font );
    lblCTime5->setText( tr( "" ) );
    lblCTime5->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    Layout2->addWidget( lblCTime5, 4, 1 );

    lblCTime2 = new QLabel( buttonWidget, "lblCTime2" );
    lblCTime2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, lblCTime2->sizePolicy().hasHeightForWidth() ) );
    QFont lblCTime2_font(  lblCTime2->font() );
    lblCTime2_font.setPointSize( 10 );
    lblCTime2->setFont( lblCTime2_font );
    lblCTime2->setText( tr( "" ) );
    lblCTime2->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    Layout2->addWidget( lblCTime2, 1, 1 );

    lblCTime1 = new QLabel( buttonWidget, "lblCTime1" );
    lblCTime1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, lblCTime1->sizePolicy().hasHeightForWidth() ) );
    QFont lblCTime1_font(  lblCTime1->font() );
    lblCTime1_font.setPointSize( 10 );
    lblCTime1->setFont( lblCTime1_font );
    lblCTime1->setText( tr( "" ) );
    lblCTime1->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    Layout2->addWidget( lblCTime1, 0, 1 );

    cmdCity3 = new QToolButton( buttonWidget, "cmdCity3" );
    cmdCity3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, cmdCity3->sizePolicy().hasHeightForWidth() ) );
    QFont cmdCity3_font(  cmdCity3->font() );
    cmdCity3_font.setBold( TRUE );
    cmdCity3->setFont( cmdCity3_font );
    cmdCity3->setFocusPolicy( QToolButton::TabFocus );
    cmdCity3->setText( tr( "" ) );
    cmdCity3->setToggleButton( TRUE );
    cmdCity3->setToggleButton( TRUE );

    Layout2->addWidget( cmdCity3, 2, 0 );

    lblCTime8 = new QLabel( buttonWidget, "lblCTime8" );
    lblCTime8->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, lblCTime8->sizePolicy().hasHeightForWidth() ) );
    QFont lblCTime8_font(  lblCTime8->font() );
    lblCTime8_font.setPointSize( 10 );
    lblCTime8->setFont( lblCTime8_font );
    lblCTime8->setText( tr( "" ) );
    lblCTime8->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    Layout2->addWidget( lblCTime8, 1, 3 );

    cmdCity1 = new QToolButton( buttonWidget, "cmdCity1" );
    cmdCity1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, cmdCity1->sizePolicy().hasHeightForWidth() ) );
    QFont cmdCity1_font(  cmdCity1->font() );
    cmdCity1_font.setBold( TRUE );
    cmdCity1->setFont( cmdCity1_font );
    cmdCity1->setFocusPolicy( QToolButton::TabFocus );
    cmdCity1->setText( tr( "" ) );
    cmdCity1->setToggleButton( TRUE );
    cmdCity1->setToggleButton( TRUE );

    Layout2->addWidget( cmdCity1, 0, 0 );

    lblCTime7 = new QLabel( buttonWidget, "lblCTime7" );
    lblCTime7->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, lblCTime7->sizePolicy().hasHeightForWidth() ) );
    QFont lblCTime7_font(  lblCTime7->font() );
    lblCTime7_font.setPointSize( 10 );
    lblCTime7->setFont( lblCTime7_font );
    lblCTime7->setText( tr( "" ) );
    lblCTime7->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    Layout2->addWidget( lblCTime7, 0, 3 );

    lblCTime4 = new QLabel( buttonWidget, "lblCTime4" );
    QFont lblCTime4_font(  lblCTime4->font() );
    lblCTime4_font.setPointSize( 10 );
    lblCTime4->setFont( lblCTime4_font );
    lblCTime4->setText( tr( "" ) );
    lblCTime4->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    Layout2->addWidget( lblCTime4, 3, 1 );

    cmdCity5 = new QToolButton( buttonWidget, "cmdCity5" );
    cmdCity5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, cmdCity5->sizePolicy().hasHeightForWidth() ) );
    QFont cmdCity5_font(  cmdCity5->font() );
    cmdCity5_font.setBold( TRUE );
    cmdCity5->setFont( cmdCity5_font );
    cmdCity5->setFocusPolicy( QToolButton::TabFocus );
    cmdCity5->setText( tr( "" ) );
    cmdCity5->setToggleButton( TRUE );
    cmdCity5->setToggleButton( TRUE );

    Layout2->addWidget( cmdCity5, 4, 0 );

    lblCTime3 = new QLabel( buttonWidget, "lblCTime3" );
    QFont lblCTime3_font(  lblCTime3->font() );
    lblCTime3_font.setPointSize( 10 );
    lblCTime3->setFont( lblCTime3_font );
    lblCTime3->setText( tr( "" ) );
    lblCTime3->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    Layout2->addWidget( lblCTime3, 2, 1 );

    cmdCity9 = new QToolButton( buttonWidget, "cmdCity9" );
    cmdCity9->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, cmdCity9->sizePolicy().hasHeightForWidth() ) );
    QFont cmdCity9_font(  cmdCity9->font() );
    cmdCity9_font.setBold( TRUE );
    cmdCity9->setFont( cmdCity9_font );
    cmdCity9->setFocusPolicy( QToolButton::TabFocus );
    cmdCity9->setText( tr( "" ) );
    cmdCity9->setToggleButton( TRUE );
    cmdCity9->setToggleButton( TRUE );

    Layout2->addWidget( cmdCity9, 2, 2 );
    CityTimeBaseLayout->addWidget( buttonWidget );

    // signals and slots connections
    connect( frmMap, SIGNAL( signalTz(const QString &, const QString &) ), this, SLOT( slotNewTz(const QString &, const QString &) ) );
    connect( cmdCity1, SIGNAL( toggled(bool) ), cmdCity2, SLOT( setDisabled(bool) ) );
    connect( cmdCity1, SIGNAL( toggled(bool) ), cmdCity3, SLOT( setDisabled(bool) ) );
    connect( cmdCity1, SIGNAL( toggled(bool) ), cmdCity4, SLOT( setDisabled(bool) ) );
    connect( cmdCity1, SIGNAL( toggled(bool) ), cmdCity7, SLOT( setDisabled(bool) ) );
    connect( cmdCity1, SIGNAL( toggled(bool) ), cmdCity8, SLOT( setDisabled(bool) ) );
    connect( cmdCity2, SIGNAL( toggled(bool) ), cmdCity1, SLOT( setDisabled(bool) ) );
    connect( cmdCity2, SIGNAL( toggled(bool) ), cmdCity3, SLOT( setDisabled(bool) ) );
    connect( cmdCity2, SIGNAL( toggled(bool) ), cmdCity4, SLOT( setDisabled(bool) ) );
    connect( cmdCity4, SIGNAL( toggled(bool) ), cmdCity7, SLOT( setDisabled(bool) ) );
    connect( cmdCity2, SIGNAL( toggled(bool) ), cmdCity8, SLOT( setDisabled(bool) ) );
    connect( cmdCity2, SIGNAL( toggled(bool) ), cmdCity7, SLOT( setDisabled(bool) ) );
    connect( cmdCity3, SIGNAL( toggled(bool) ), cmdCity1, SLOT( setDisabled(bool) ) );
    connect( cmdCity3, SIGNAL( toggled(bool) ), cmdCity2, SLOT( setDisabled(bool) ) );
    connect( cmdCity3, SIGNAL( toggled(bool) ), cmdCity4, SLOT( setDisabled(bool) ) );
    connect( cmdCity3, SIGNAL( toggled(bool) ), cmdCity7, SLOT( setDisabled(bool) ) );
    connect( cmdCity3, SIGNAL( toggled(bool) ), cmdCity8, SLOT( setDisabled(bool) ) );
    connect( cmdCity4, SIGNAL( toggled(bool) ), cmdCity1, SLOT( setDisabled(bool) ) );
    connect( cmdCity4, SIGNAL( toggled(bool) ), cmdCity2, SLOT( setDisabled(bool) ) );
    connect( cmdCity4, SIGNAL( toggled(bool) ), cmdCity3, SLOT( setDisabled(bool) ) );
    connect( cmdCity4, SIGNAL( toggled(bool) ), cmdCity8, SLOT( setDisabled(bool) ) );
    connect( cmdCity7, SIGNAL( toggled(bool) ), cmdCity1, SLOT( setDisabled(bool) ) );
    connect( cmdCity7, SIGNAL( toggled(bool) ), cmdCity2, SLOT( setDisabled(bool) ) );
    connect( cmdCity7, SIGNAL( toggled(bool) ), cmdCity3, SLOT( setDisabled(bool) ) );
    connect( cmdCity7, SIGNAL( toggled(bool) ), cmdCity4, SLOT( setDisabled(bool) ) );
    connect( cmdCity7, SIGNAL( toggled(bool) ), cmdCity8, SLOT( setDisabled(bool) ) );
    connect( cmdCity8, SIGNAL( toggled(bool) ), cmdCity1, SLOT( setDisabled(bool) ) );
    connect( cmdCity8, SIGNAL( toggled(bool) ), cmdCity2, SLOT( setDisabled(bool) ) );
    connect( cmdCity8, SIGNAL( toggled(bool) ), cmdCity3, SLOT( setDisabled(bool) ) );
    connect( cmdCity8, SIGNAL( toggled(bool) ), cmdCity4, SLOT( setDisabled(bool) ) );
    connect( cmdCity8, SIGNAL( toggled(bool) ), cmdCity7, SLOT( setDisabled(bool) ) );
    connect( cmdCity1, SIGNAL( toggled(bool) ), cmdCity5, SLOT( setDisabled(bool) ) );
    connect( cmdCity2, SIGNAL( toggled(bool) ), cmdCity5, SLOT( setDisabled(bool) ) );
    connect( cmdCity3, SIGNAL( toggled(bool) ), cmdCity5, SLOT( setDisabled(bool) ) );
    connect( cmdCity4, SIGNAL( toggled(bool) ), cmdCity5, SLOT( setDisabled(bool) ) );
    connect( cmdCity7, SIGNAL( toggled(bool) ), cmdCity5, SLOT( setDisabled(bool) ) );
    connect( cmdCity8, SIGNAL( toggled(bool) ), cmdCity5, SLOT( setDisabled(bool) ) );
    connect( cmdCity5, SIGNAL( toggled(bool) ), cmdCity1, SLOT( setDisabled(bool) ) );
    connect( cmdCity5, SIGNAL( toggled(bool) ), cmdCity2, SLOT( setDisabled(bool) ) );
    connect( cmdCity5, SIGNAL( toggled(bool) ), cmdCity3, SLOT( setDisabled(bool) ) );
    connect( cmdCity5, SIGNAL( toggled(bool) ), cmdCity4, SLOT( setDisabled(bool) ) );
    connect( cmdCity5, SIGNAL( toggled(bool) ), cmdCity7, SLOT( setDisabled(bool) ) );
    connect( cmdCity5, SIGNAL( toggled(bool) ), cmdCity8, SLOT( setDisabled(bool) ) );
    connect( cmdCity1, SIGNAL( toggled(bool) ), cmdCity6, SLOT( setDisabled(bool) ) );
    connect( cmdCity2, SIGNAL( toggled(bool) ), cmdCity6, SLOT( setDisabled(bool) ) );
    connect( cmdCity3, SIGNAL( toggled(bool) ), cmdCity6, SLOT( setDisabled(bool) ) );
    connect( cmdCity4, SIGNAL( toggled(bool) ), cmdCity6, SLOT( setDisabled(bool) ) );
    connect( cmdCity5, SIGNAL( toggled(bool) ), cmdCity6, SLOT( setDisabled(bool) ) );
    connect( cmdCity6, SIGNAL( toggled(bool) ), cmdCity1, SLOT( setDisabled(bool) ) );
    connect( cmdCity6, SIGNAL( toggled(bool) ), cmdCity2, SLOT( setDisabled(bool) ) );
    connect( cmdCity6, SIGNAL( toggled(bool) ), cmdCity3, SLOT( setDisabled(bool) ) );
    connect( cmdCity6, SIGNAL( toggled(bool) ), cmdCity4, SLOT( setDisabled(bool) ) );
    connect( cmdCity6, SIGNAL( toggled(bool) ), cmdCity5, SLOT( setDisabled(bool) ) );
    connect( cmdCity6, SIGNAL( toggled(bool) ), cmdCity7, SLOT( setDisabled(bool) ) );
    connect( cmdCity6, SIGNAL( toggled(bool) ), cmdCity8, SLOT( setDisabled(bool) ) );
    connect( cmdCity6, SIGNAL( toggled(bool) ), cmdCity9, SLOT( setDisabled(bool) ) );
    connect( cmdCity7, SIGNAL( toggled(bool) ), cmdCity6, SLOT( setDisabled(bool) ) );
    connect( cmdCity8, SIGNAL( toggled(bool) ), cmdCity6, SLOT( setDisabled(bool) ) );
    connect( cmdCity9, SIGNAL( toggled(bool) ), cmdCity1, SLOT( setDisabled(bool) ) );
    connect( cmdCity9, SIGNAL( toggled(bool) ), cmdCity3, SLOT( setDisabled(bool) ) );
    connect( cmdCity9, SIGNAL( toggled(bool) ), cmdCity4, SLOT( setDisabled(bool) ) );
    connect( cmdCity9, SIGNAL( toggled(bool) ), cmdCity5, SLOT( setDisabled(bool) ) );
    connect( cmdCity9, SIGNAL( toggled(bool) ), cmdCity6, SLOT( setDisabled(bool) ) );
    connect( cmdCity9, SIGNAL( toggled(bool) ), cmdCity7, SLOT( setDisabled(bool) ) );
    connect( cmdCity9, SIGNAL( toggled(bool) ), cmdCity8, SLOT( setDisabled(bool) ) );
    connect( cmdCity1, SIGNAL( toggled(bool) ), cmdCity9, SLOT( setDisabled(bool) ) );
    connect( cmdCity2, SIGNAL( toggled(bool) ), cmdCity9, SLOT( setDisabled(bool) ) );
    connect( cmdCity3, SIGNAL( toggled(bool) ), cmdCity9, SLOT( setDisabled(bool) ) );
    connect( cmdCity4, SIGNAL( toggled(bool) ), cmdCity9, SLOT( setDisabled(bool) ) );
    connect( cmdCity5, SIGNAL( toggled(bool) ), cmdCity9, SLOT( setDisabled(bool) ) );
    connect( cmdCity7, SIGNAL( toggled(bool) ), cmdCity9, SLOT( setDisabled(bool) ) );
    connect( cmdCity8, SIGNAL( toggled(bool) ), cmdCity9, SLOT( setDisabled(bool) ) );
    connect( cmdCity9, SIGNAL( toggled(bool) ), cmdCity2, SLOT( setDisabled(bool) ) );
    connect( cmdCity1, SIGNAL( toggled(bool) ), this, SLOT( beginNewTz() ) );
    connect( cmdCity2, SIGNAL( toggled(bool) ), this, SLOT( beginNewTz() ) );
    connect( cmdCity7, SIGNAL( toggled(bool) ), this, SLOT( beginNewTz() ) );
    connect( cmdCity3, SIGNAL( toggled(bool) ), this, SLOT( beginNewTz() ) );
    connect( cmdCity4, SIGNAL( toggled(bool) ), this, SLOT( beginNewTz() ) );
    connect( cmdCity5, SIGNAL( toggled(bool) ), this, SLOT( beginNewTz() ) );
    connect( cmdCity6, SIGNAL( toggled(bool) ), this, SLOT( beginNewTz() ) );
    connect( cmdCity8, SIGNAL( toggled(bool) ), this, SLOT( beginNewTz() ) );
    connect( cmdCity9, SIGNAL( toggled(bool) ), this, SLOT( beginNewTz() ) );

    // tab order
    setTabOrder( cmdCity1, cmdCity2 );
    setTabOrder( cmdCity2, cmdCity3 );
    setTabOrder( cmdCity3, cmdCity4 );
    setTabOrder( cmdCity4, cmdCity5 );
    setTabOrder( cmdCity5, cmdCity6 );
    setTabOrder( cmdCity6, cmdCity7 );
    setTabOrder( cmdCity7, cmdCity8 );
    setTabOrder( cmdCity8, cmdCity9 );
    setTabOrder( cmdCity9, frmMap );
}

/*
 *  Destroys the object and frees any allocated resources
 */
CityTimeBase::~CityTimeBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Main event handler. Reimplemented to handle application
 *  font changes
 */
bool CityTimeBase::event( QEvent* ev )
{
    bool ret = QWidget::event( ev );
    if ( ev->type() == QEvent::ApplicationFontChange ) {
	QFont cmdCity7_font(  cmdCity7->font() );
	cmdCity7_font.setBold( TRUE );
	cmdCity7->setFont( cmdCity7_font );
	QFont lblCTime9_font(  lblCTime9->font() );
	lblCTime9_font.setPointSize( 10 );
	lblCTime9->setFont( lblCTime9_font );
	QFont cmdCity8_font(  cmdCity8->font() );
	cmdCity8_font.setBold( TRUE );
	cmdCity8->setFont( cmdCity8_font );
	QFont cmdCity2_font(  cmdCity2->font() );
	cmdCity2_font.setBold( TRUE );
	cmdCity2->setFont( cmdCity2_font );
	QFont lblCTime6_font(  lblCTime6->font() );
	lblCTime6_font.setPointSize( 10 );
	lblCTime6->setFont( lblCTime6_font );
	QFont cmdCity6_font(  cmdCity6->font() );
	cmdCity6_font.setBold( TRUE );
	cmdCity6->setFont( cmdCity6_font );
	QFont cmdCity4_font(  cmdCity4->font() );
	cmdCity4_font.setBold( TRUE );
	cmdCity4->setFont( cmdCity4_font );
	QFont lblCTime5_font(  lblCTime5->font() );
	lblCTime5_font.setPointSize( 10 );
	lblCTime5->setFont( lblCTime5_font );
	QFont lblCTime2_font(  lblCTime2->font() );
	lblCTime2_font.setPointSize( 10 );
	lblCTime2->setFont( lblCTime2_font );
	QFont lblCTime1_font(  lblCTime1->font() );
	lblCTime1_font.setPointSize( 10 );
	lblCTime1->setFont( lblCTime1_font );
	QFont cmdCity3_font(  cmdCity3->font() );
	cmdCity3_font.setBold( TRUE );
	cmdCity3->setFont( cmdCity3_font );
	QFont lblCTime8_font(  lblCTime8->font() );
	lblCTime8_font.setPointSize( 10 );
	lblCTime8->setFont( lblCTime8_font );
	QFont cmdCity1_font(  cmdCity1->font() );
	cmdCity1_font.setBold( TRUE );
	cmdCity1->setFont( cmdCity1_font );
	QFont lblCTime7_font(  lblCTime7->font() );
	lblCTime7_font.setPointSize( 10 );
	lblCTime7->setFont( lblCTime7_font );
	QFont lblCTime4_font(  lblCTime4->font() );
	lblCTime4_font.setPointSize( 10 );
	lblCTime4->setFont( lblCTime4_font );
	QFont cmdCity5_font(  cmdCity5->font() );
	cmdCity5_font.setBold( TRUE );
	cmdCity5->setFont( cmdCity5_font );
	QFont lblCTime3_font(  lblCTime3->font() );
	lblCTime3_font.setPointSize( 10 );
	lblCTime3->setFont( lblCTime3_font );
	QFont cmdCity9_font(  cmdCity9->font() );
	cmdCity9_font.setBold( TRUE );
	cmdCity9->setFont( cmdCity9_font );
    }
    return ret;
}

void CityTimeBase::beginNewTz()
{
    qWarning( "CityTimeBase::beginNewTz(): Not implemented yet!" );
}

void CityTimeBase::slotNewTz(const QString &, const QString &)
{
    qWarning( "CityTimeBase::slotNewTz(const QString &, const QString &): Not implemented yet!" );
}

