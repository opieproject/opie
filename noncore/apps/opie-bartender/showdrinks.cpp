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
#include "showdrinks.h"


#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>

//extern Bartender  *bart;

Show_Drink::Show_Drink( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl ) {
    if ( !name )
    setName( drinkName);

    drinkName = name;
    setCaption( drinkName );

    Layout5 = new QGridLayout( this); 
    Layout5->setSpacing( 6 );
    Layout5->setMargin( 4 );

    
      editDrinkButton = new QPushButton( tr("Edit"), this,"editBtn");
      Layout5->addMultiCellWidget(  editDrinkButton, 0, 0, 0, 0 );
      connect(editDrinkButton, SIGNAL(clicked()), this, SLOT(slotEditDrink()));
            
    MultiLineEdit1 = new QMultiLineEdit( this, "MultiLineEdit1" );
    MultiLineEdit1->setWordWrap(QMultiLineEdit::WidgetWidth);
    Layout5->addMultiCellWidget(  MultiLineEdit1, 1, 3, 0, 3 );
}

Show_Drink::~Show_Drink() {
}

void Show_Drink::slotEditDrink() {
   reject();
}
