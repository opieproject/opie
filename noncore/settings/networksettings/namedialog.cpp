/****************************************************************************
** Form implementation generated from reading ui file 'namedialog.ui'
**
** Created: Sat Dec 15 04:36:20 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "namedialog.h"

#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a NameDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
NameDialog::NameDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "NameDialog" );
    resize( 176, 65 ); 
    setCaption( tr( "Enter A Name" ) );
    NameDialogLayout = new QGridLayout( this ); 
    NameDialogLayout->setSpacing( 6 );
    NameDialogLayout->setMargin( 5 );

    Name = new QLineEdit( this, "Name" );

    NameDialogLayout->addMultiCellWidget( Name, 0, 0, 0, 1 );

    cancel = new QPushButton( this, "Cancel" );
    cancel->setText( tr( "&Cancel" ) );

    NameDialogLayout->addWidget( cancel, 1, 1 );

    ok = new QPushButton( this, "ok" );
    ok->setText( tr( "&Ok" ) );

    NameDialogLayout->addWidget( ok, 1, 0 );

  connect( ok, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( cancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( Name, SIGNAL(returnPressed () ), this, SLOT(accept() ) );
}

QString NameDialog::go(){
  if( exec() ) //pressed OK
    return (Name->text());
  else //pressed cancel
    return QString();
}

