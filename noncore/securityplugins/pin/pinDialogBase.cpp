/****************************************************************************
** Form implementation generated from reading ui file 'pinDialogBase.ui'
**
** Created: Mon Jun 14 11:01:08 2004
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "pinDialogBase.h"

#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a PinDialogBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
PinDialogBase::PinDialogBase( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "PinDialogBase" );
    resize( 231, 293 ); 
    QFont f( font() );
    f.setPointSize( 24 );
    setFont( f ); 
    setCaption( tr( "PinDialogBase" ) );

    button_4 = new QLabel( this, "button_4" );
    button_4->setGeometry( QRect( 11, 138, 68, 44 ) ); 
    button_4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, button_4->sizePolicy().hasHeightForWidth() ) );
    button_4->setFrameShape( QLabel::Box );
    button_4->setFrameShadow( QLabel::Raised );
    button_4->setText( tr( "4" ) );
    button_4->setAlignment( int( QLabel::AlignCenter ) );

    button_7 = new QLabel( this, "button_7" );
    button_7->setGeometry( QRect( 11, 188, 68, 44 ) ); 
    button_7->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, button_7->sizePolicy().hasHeightForWidth() ) );
    button_7->setFrameShape( QLabel::Box );
    button_7->setFrameShadow( QLabel::Raised );
    button_7->setText( tr( "7" ) );
    button_7->setAlignment( int( QLabel::AlignCenter ) );

    prompt = new QLabel( this, "prompt" );
    prompt->setGeometry( QRect( 11, 11, 217, 25 ) ); 
    QFont prompt_font(  prompt->font() );
    prompt_font.setPointSize( 18 );
    prompt->setFont( prompt_font ); 
    prompt->setText( tr( "Enter your PIN" ) );

    display = new QLineEdit( this, "display" );
    display->setEnabled( FALSE );
    display->setGeometry( QRect( 11, 42, 217, 40 ) ); 
    QPalette pal;
    QColorGroup cg;
    cg.setColor( QColorGroup::Foreground, black );
    cg.setColor( QColorGroup::Button, QColor( 228, 228, 228) );
    cg.setColor( QColorGroup::Light, white );
    cg.setColor( QColorGroup::Midlight, QColor( 242, 242, 242) );
    cg.setColor( QColorGroup::Dark, QColor( 78, 78, 78) );
    cg.setColor( QColorGroup::Mid, QColor( 183, 183, 183) );
    cg.setColor( QColorGroup::Text, black );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, black );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 220, 220, 220) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, QColor( 10, 95, 137) );
    cg.setColor( QColorGroup::HighlightedText, white );
    pal.setActive( cg );
    cg.setColor( QColorGroup::Foreground, black );
    cg.setColor( QColorGroup::Button, QColor( 228, 228, 228) );
    cg.setColor( QColorGroup::Light, white );
    cg.setColor( QColorGroup::Midlight, QColor( 242, 242, 242) );
    cg.setColor( QColorGroup::Dark, QColor( 78, 78, 78) );
    cg.setColor( QColorGroup::Mid, QColor( 183, 183, 183) );
    cg.setColor( QColorGroup::Text, black );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, black );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 220, 220, 220) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, QColor( 10, 95, 137) );
    cg.setColor( QColorGroup::HighlightedText, white );
    pal.setInactive( cg );
    cg.setColor( QColorGroup::Foreground, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Button, QColor( 228, 228, 228) );
    cg.setColor( QColorGroup::Light, white );
    cg.setColor( QColorGroup::Midlight, white );
    cg.setColor( QColorGroup::Dark, QColor( 114, 114, 114) );
    cg.setColor( QColorGroup::Mid, QColor( 152, 152, 152) );
    cg.setColor( QColorGroup::Text, black );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, black );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 220, 220, 220) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
    cg.setColor( QColorGroup::HighlightedText, white );
    pal.setDisabled( cg );
    display->setPalette( pal );
    display->setEchoMode( QLineEdit::Password );

    button_1 = new QLabel( this, "button_1" );
    button_1->setGeometry( QRect( 11, 88, 68, 44 ) ); 
    button_1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, button_1->sizePolicy().hasHeightForWidth() ) );
    button_1->setFrameShape( QLabel::Box );
    button_1->setFrameShadow( QLabel::Raised );
    button_1->setText( tr( "1" ) );
    button_1->setAlignment( int( QLabel::AlignCenter ) );

    button_2 = new QLabel( this, "button_2" );
    button_2->setGeometry( QRect( 85, 88, 69, 44 ) ); 
    button_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, button_2->sizePolicy().hasHeightForWidth() ) );
    button_2->setFrameShape( QLabel::Box );
    button_2->setFrameShadow( QLabel::Raised );
    button_2->setText( tr( "2" ) );
    button_2->setAlignment( int( QLabel::AlignCenter ) );

    button_5 = new QLabel( this, "button_5" );
    button_5->setGeometry( QRect( 85, 138, 69, 44 ) ); 
    button_5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, button_5->sizePolicy().hasHeightForWidth() ) );
    button_5->setFrameShape( QLabel::Box );
    button_5->setFrameShadow( QLabel::Raised );
    button_5->setText( tr( "5" ) );
    button_5->setAlignment( int( QLabel::AlignCenter ) );

    button_6 = new QLabel( this, "button_6" );
    button_6->setGeometry( QRect( 160, 138, 68, 44 ) ); 
    button_6->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, button_6->sizePolicy().hasHeightForWidth() ) );
    button_6->setFrameShape( QLabel::Box );
    button_6->setFrameShadow( QLabel::Raised );
    button_6->setText( tr( "6" ) );
    button_6->setAlignment( int( QLabel::AlignCenter ) );

    button_3 = new QLabel( this, "button_3" );
    button_3->setGeometry( QRect( 160, 88, 68, 44 ) ); 
    button_3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, button_3->sizePolicy().hasHeightForWidth() ) );
    button_3->setFrameShape( QLabel::Box );
    button_3->setFrameShadow( QLabel::Raised );
    button_3->setText( tr( "3" ) );
    button_3->setAlignment( int( QLabel::AlignCenter ) );

    button_0 = new QLabel( this, "button_0" );
    button_0->setGeometry( QRect( 11, 238, 68, 44 ) ); 
    button_0->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, button_0->sizePolicy().hasHeightForWidth() ) );
    button_0->setFrameShape( QLabel::Box );
    button_0->setFrameShadow( QLabel::Raised );
    button_0->setText( tr( "0" ) );
    button_0->setAlignment( int( QLabel::AlignCenter ) );

    button_8 = new QLabel( this, "button_8" );
    button_8->setGeometry( QRect( 85, 188, 69, 44 ) ); 
    button_8->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, button_8->sizePolicy().hasHeightForWidth() ) );
    button_8->setFrameShape( QLabel::Box );
    button_8->setFrameShadow( QLabel::Raised );
    button_8->setText( tr( "8" ) );
    button_8->setAlignment( int( QLabel::AlignCenter ) );

    button_9 = new QLabel( this, "button_9" );
    button_9->setGeometry( QRect( 160, 188, 68, 44 ) ); 
    button_9->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, button_9->sizePolicy().hasHeightForWidth() ) );
    button_9->setFrameShape( QLabel::Box );
    button_9->setFrameShadow( QLabel::Raised );
    button_9->setText( tr( "9" ) );
    button_9->setAlignment( int( QLabel::AlignCenter ) );

    button_OK = new QLabel( this, "button_OK" );
    button_OK->setGeometry( QRect( 160, 238, 68, 44 ) ); 
    button_OK->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, button_OK->sizePolicy().hasHeightForWidth() ) );
    cg.setColor( QColorGroup::Foreground, black );
    cg.setColor( QColorGroup::Button, QColor( 192, 192, 192) );
    cg.setColor( QColorGroup::Light, white );
    cg.setColor( QColorGroup::Midlight, QColor( 223, 223, 223) );
    cg.setColor( QColorGroup::Dark, QColor( 96, 96, 96) );
    cg.setColor( QColorGroup::Mid, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Text, black );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, black );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 0, 170, 0) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, black );
    cg.setColor( QColorGroup::HighlightedText, white );
    pal.setActive( cg );
    cg.setColor( QColorGroup::Foreground, black );
    cg.setColor( QColorGroup::Button, QColor( 192, 192, 192) );
    cg.setColor( QColorGroup::Light, white );
    cg.setColor( QColorGroup::Midlight, QColor( 220, 220, 220) );
    cg.setColor( QColorGroup::Dark, QColor( 96, 96, 96) );
    cg.setColor( QColorGroup::Mid, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Text, black );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, black );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 0, 170, 0) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, black );
    cg.setColor( QColorGroup::HighlightedText, white );
    pal.setInactive( cg );
    cg.setColor( QColorGroup::Foreground, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Button, QColor( 192, 192, 192) );
    cg.setColor( QColorGroup::Light, white );
    cg.setColor( QColorGroup::Midlight, QColor( 220, 220, 220) );
    cg.setColor( QColorGroup::Dark, QColor( 96, 96, 96) );
    cg.setColor( QColorGroup::Mid, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Text, black );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 0, 170, 0) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, black );
    cg.setColor( QColorGroup::HighlightedText, white );
    pal.setDisabled( cg );
    button_OK->setPalette( pal );
    button_OK->setFrameShape( QLabel::Box );
    button_OK->setFrameShadow( QLabel::Raised );
    button_OK->setText( tr( "Enter" ) );
    button_OK->setAlignment( int( QLabel::AlignCenter ) );

    button_Skip = new QLabel( this, "button_Skip" );
    button_Skip->setGeometry( QRect( 85, 238, 69, 44 ) ); 
    button_Skip->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, button_Skip->sizePolicy().hasHeightForWidth() ) );
    cg.setColor( QColorGroup::Foreground, black );
    cg.setColor( QColorGroup::Button, QColor( 192, 192, 192) );
    cg.setColor( QColorGroup::Light, white );
    cg.setColor( QColorGroup::Midlight, QColor( 223, 223, 223) );
    cg.setColor( QColorGroup::Dark, QColor( 96, 96, 96) );
    cg.setColor( QColorGroup::Mid, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Text, black );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, black );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 255, 255, 127) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, black );
    cg.setColor( QColorGroup::HighlightedText, white );
    pal.setActive( cg );
    cg.setColor( QColorGroup::Foreground, black );
    cg.setColor( QColorGroup::Button, QColor( 192, 192, 192) );
    cg.setColor( QColorGroup::Light, white );
    cg.setColor( QColorGroup::Midlight, QColor( 220, 220, 220) );
    cg.setColor( QColorGroup::Dark, QColor( 96, 96, 96) );
    cg.setColor( QColorGroup::Mid, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Text, black );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, black );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 255, 255, 127) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, black );
    cg.setColor( QColorGroup::HighlightedText, white );
    pal.setInactive( cg );
    cg.setColor( QColorGroup::Foreground, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Button, QColor( 192, 192, 192) );
    cg.setColor( QColorGroup::Light, white );
    cg.setColor( QColorGroup::Midlight, QColor( 220, 220, 220) );
    cg.setColor( QColorGroup::Dark, QColor( 96, 96, 96) );
    cg.setColor( QColorGroup::Mid, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Text, black );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 255, 255, 127) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, black );
    cg.setColor( QColorGroup::HighlightedText, white );
    pal.setDisabled( cg );
    button_Skip->setPalette( pal );
    button_Skip->setFrameShape( QLabel::Box );
    button_Skip->setFrameShadow( QLabel::Raised );
    button_Skip->setText( tr( "Skip" ) );
    button_Skip->setAlignment( int( QLabel::AlignCenter ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
PinDialogBase::~PinDialogBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*  
 *  Main event handler. Reimplemented to handle application
 *  font changes
 */
bool PinDialogBase::event( QEvent* ev )
{
    bool ret = QWidget::event( ev ); 
    if ( ev->type() == QEvent::ApplicationFontChange ) {
	QFont prompt_font(  prompt->font() );
	prompt_font.setPointSize( 18 );
	prompt->setFont( prompt_font ); 
    }
    return ret;
}

