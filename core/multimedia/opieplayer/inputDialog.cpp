/****************************************************************************
** Form implementation generated from reading ui file 'inputDialog.ui'
**
** Created: Sat Mar 2 07:55:03 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "inputDialog.h"

#include <qpe/resource.h>

#include <opie/ofiledialog.h>

#include <qlineedit.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qpushbutton.h>
#include <qwhatsthis.h>

InputDialog::InputDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
  setName( "InputDialog" );
    resize( 234, 115); 
    setMaximumSize( QSize( 240, 40));
    setCaption( tr(name ) );

    QPushButton *browserButton;
    browserButton = new QPushButton( Resource::loadIconSet("fileopen"),"",this,"BrowseButton");
    browserButton->setGeometry( QRect( 205, 10, 22, 22));
    connect( browserButton, SIGNAL(released()),this,SLOT(browse()));
    LineEdit1 = new QLineEdit( this, "LineEdit1" );
    LineEdit1->setGeometry( QRect( 4, 10, 190, 22 ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
InputDialog::~InputDialog()
{
    inputText= LineEdit1->text();
  
}

void InputDialog::browse() {

    QString str = OFileDialog::getOpenFileName( 1,"/","", "audio/*;video/*;playlist/plain;audio/x-mpegurl", 0 );
    LineEdit1->setText(str);
}

