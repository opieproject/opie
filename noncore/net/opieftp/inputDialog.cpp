/****************************************************************************
   inputDialog.cpp  
                             -------------------
** Created: Sat Mar 9 23:33:09 2002
    copyright            : (C) 2002 by ljp
    email                : ljp@llornkcor.com
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "inputDialog.h"

#include <qlineedit.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>


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

InputDialog::~InputDialog()
{
    inputText= LineEdit1->text();
  
}

