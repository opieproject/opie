/****************************************************************************
** Form implementation generated from reading ui file 'qchecknamebase.ui'
**
** Created: Wed Sep 18 09:40:57 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "qchecknamebase.h"

#include <qlabel.h>
#include <qpushbutton.h>
#include "qrestrictedline.h"
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a QCheckNameBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
QCheckNameBase::QCheckNameBase( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
  setName( "QCheckNameBase" );
    resize( 228, 108 ); 
    setCaption( tr( "Check Book Name" ) );

    TextLabel3 = new QLabel( this, "TextLabel3" );
    TextLabel3->setGeometry( QRect( 10, 5, 70, 16 ) ); 
    QFont TextLabel3_font(  TextLabel3->font() );
    TextLabel3_font.setFamily( "BDF-helvetica" );
    TextLabel3_font.setPointSize( 19 );
    TextLabel3_font.setBold( TRUE );
    TextLabel3->setFont( TextLabel3_font ); 
    TextLabel3->setText( tr( "Name..." ) );

    TextLabel4 = new QLabel( this, "TextLabel4" );
    TextLabel4->setGeometry( QRect( 10, 25, 210, 25 ) ); 
    TextLabel4->setText( tr( "Please name your check book.\n(limit: 15 characters):" ) );
    TextLabel4->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter | QLabel::AlignLeft ) );

    cmdDone = new QPushButton( this, "cmdDone" );
    cmdDone->setGeometry( QRect( 75, 80, 75, 25 ) ); 
    cmdDone->setText( tr( "&Done" ) );

    leText = new QRestrictedLine( this, "leText" );
    leText->setGeometry( QRect( 5, 51, 216, 25 ) );
    leText->setFocus();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
QCheckNameBase::~QCheckNameBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*  
 *  Main event handler. Reimplemented to handle application
 *  font changes
 */
bool QCheckNameBase::event( QEvent* ev )
{
    bool ret = QDialog::event( ev ); 
    if ( ev->type() == QEvent::ApplicationFontChange ) {
  QFont TextLabel3_font(  TextLabel3->font() );
  TextLabel3_font.setFamily( "BDF-helvetica" );
  TextLabel3_font.setPointSize( 19 );
  TextLabel3_font.setBold( TRUE );
  TextLabel3->setFont( TextLabel3_font ); 
    }
    return ret;
}

