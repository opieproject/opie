/****************************************************************************
** Form implementation generated from reading ui file 'app.ui'
**
** Created: Fri May 3 14:38:15 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "app.h"

#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a AppEdit which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
AppEdit::AppEdit( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
  setName( "AppEdit" );
    resize( 211, 241 );
    setCaption( tr( "Application" ) );
    AppEditLayout = new QGridLayout( this );
    AppEditLayout->setSpacing( 6 );
    AppEditLayout->setMargin( 11 );

    TextLabel3 = new QLabel( this, "TextLabel3" );
    TextLabel3->setText( tr( "Exec" ) );

    AppEditLayout->addWidget( TextLabel3, 2, 0 );

    TextLabel4 = new QLabel( this, "TextLabel4" );
    TextLabel4->setText( tr( "Comment" ) );

    AppEditLayout->addWidget( TextLabel4, 3, 0 );

    TextLabel2 = new QLabel( this, "TextLabel2" );
    TextLabel2->setText( tr( "Icon" ) );

    AppEditLayout->addWidget( TextLabel2, 1, 0 );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setText( tr( "Name" ) );

    AppEditLayout->addWidget( TextLabel1, 0, 0 );

    iconLineEdit = new QLineEdit( this, "iconLineEdit" );

    AppEditLayout->addWidget( iconLineEdit, 1, 1 );

    nameLineEdit = new QLineEdit( this, "nameLineEdit" );

    AppEditLayout->addWidget( nameLineEdit, 0, 1 );

    commentLineEdit = new QLineEdit( this, "commentLineEdit" );

    AppEditLayout->addWidget( commentLineEdit, 3, 1 );

    execLineEdit = new QLineEdit( this, "execLineEdit" );

    AppEditLayout->addWidget( execLineEdit, 2, 1 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    AppEditLayout->addItem( spacer, 4, 1 );
}

/*
 *  Destroys the object and frees any allocated resources
 */
AppEdit::~AppEdit()
{
    // no need to delete child widgets, Qt does it all for us
}

