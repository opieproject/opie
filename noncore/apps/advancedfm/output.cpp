/****************************************************************************
** outputEdit.cpp
**
** Copyright: Fri Apr 12 15:12:58 2002 L.J. Potter <ljp@llornkcor.com>
****************************************************************************/
#include "output.h"

#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

Output::Output( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
  setName( "Output" );
    resize( 196, 269 ); 
    setCaption( name );
    OutputLayout = new QGridLayout( this ); 
    OutputLayout->setSpacing( 6 );
    OutputLayout->setMargin( 11 );

    OutputEdit = new QMultiLineEdit( this, "OutputEdit" );

    OutputLayout->addWidget( OutputEdit, 0, 0 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
Output::~Output()
{
    // no need to delete child widgets, Qt does it all for us
}

