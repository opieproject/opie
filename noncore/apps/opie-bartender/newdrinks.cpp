/****************************************************************************
** Created: Sat Jul 20 08:23:27 2002
**      by:  L.J. Potter <ljp@llornkcor.com>
**     copyright            : (C) 2002 by ljp
    email                : ljp@llornkcor.com
    *   This program is free software; you can redistribute it and/or modify  *
    *   it under the terms of the GNU General Public License as published by  *
    *   the Free Software Foundation; either version 2 of the License, or     *
    *   (at your option) any later version.                                   *
    ***************************************************************************/
#include "newdrinks.h"


#include <qlabel.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qlayout.h>

New_Drink::New_Drink( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    QString drinkName = name;
    if ( !name )
        setName( drinkName );
    setCaption( drinkName);

    Layout5 = new QGridLayout( this); 
    Layout5->setSpacing( 6 );
    Layout5->setMargin( 4 );

//      Layout4 = new QHBoxLayout; 
//      Layout4->setSpacing( 6 );
//      Layout4->setMargin( 4 );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setText( tr( "Name" ) );
    Layout5->addMultiCellWidget( TextLabel1, 0, 0, 0, 0);

    LineEdit1 = new QLineEdit( this, "LineEdit1" );
    Layout5->addMultiCellWidget( LineEdit1, 0, 0, 1, 2);


    TextLabel2 = new QLabel( this, "TextLabel2" );
    TextLabel2->setText( tr( "Ingredients" ) );

    Layout5->addMultiCellWidget( TextLabel2, 1, 1, 0, 0);

    MultiLineEdit1 = new QMultiLineEdit( this, "MultiLineEdit1" );

    Layout5->addMultiCellWidget( MultiLineEdit1, 2, 2, 0, 2);
}

/*  
 *  Destroys the object and frees any allocated resources
 */
New_Drink::~New_Drink()
{
    // no need to delete child widgets, Qt does it all for us
}

