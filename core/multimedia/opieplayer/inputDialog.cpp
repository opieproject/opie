/****************************************************************************
** Form implementation generated from reading ui file 'inputDialog.ui'
**
** Created: Sat Mar 2 07:55:03 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "inputDialog.h"

#include <qlineedit.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a InputDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
InputDialog::InputDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
  setName( "InputDialog" );
    resize( 234, 50 ); 
    setMaximumSize( QSize( 240, 50 ) );
    setCaption( tr(name ) );

    LineEdit1 = new QLineEdit( this, "LineEdit1" );
    LineEdit1->setGeometry( QRect( 10, 10, 216, 22 ) ); 
}

/*  
 *  Destroys the object and frees any allocated resources
 */
InputDialog::~InputDialog()
{
    inputText= LineEdit1->text();
  
}

