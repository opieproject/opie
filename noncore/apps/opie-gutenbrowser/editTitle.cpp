/****************************************************************************
** Created: Thu Jan 24 08:47:37 2002
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com
****************************************************************************/
#include "editTitle.h"

#include <qlineedit.h>
#include <qpushbutton.h>

Edit_Title::Edit_Title( QWidget* parent,  const char* name, bool modal, WFlags fl)
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
  setName( "Edit_Title" );
    resize( 240, 110 ); 
    setMaximumSize( QSize( 240, 110 ) );
    setCaption( tr( "Edit Title" ) );

    LineEdit1 = new QLineEdit( this, "LineEdit1" );
    LineEdit1->setGeometry( QRect( 5, 22, 230, 22 ) );
    newTitle=name;
    LineEdit1->setText(newTitle);
    
    PushButton_Cancel = new QPushButton( this, "PushButton_Cancel" );
    PushButton_Cancel->setGeometry( QRect( 180, 80, 50, 20 ) ); 
    PushButton_Cancel->setText( tr( "Cancel" ) );

    PushButton_Apply = new QPushButton( this, "PushButton_Apply" );
    PushButton_Apply->setGeometry( QRect( 120, 80, 50, 20 ) );
    
    PushButton_Apply->setText( tr( "Apply" ) );

    // signals and slots connections
    connect( PushButton_Cancel, SIGNAL( released() ),this, SLOT( onCancel() ) );
    connect( PushButton_Apply, SIGNAL( released() ), this, SLOT( onApply() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
Edit_Title::~Edit_Title()
{
    // no need to delete child widgets, Qt does it all for us
}

void Edit_Title::onCancel() {
    reject();
}

void Edit_Title::onApply() {
  newTitle= LineEdit1->text();
  accept();
}
