/****************************************************************************
** Form implementation generated from reading ui file 'commandeditwidget.ui'
**
** Created: Sat Feb 2 11:08:25 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "commandeditwidget.h"

#include <qpushbutton.h>
#include "playlistselection.h"
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a Form1 which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
Form1::Form1( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "Form1" );
    resize( 596, 480 ); 
    setCaption( tr( "Form1" ) );
    Form1Layout = new QGridLayout( this ); 
    Form1Layout->setSpacing( 6 );
    Form1Layout->setMargin( 11 );

    MyCustomWidget1 = new PlayListSelection( this, "MyCustomWidget1" );

    Form1Layout->addWidget( MyCustomWidget1, 0, 0 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
Form1::~Form1()
{
    // no need to delete child widgets, Qt does it all for us
}

